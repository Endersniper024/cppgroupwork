#include "LLMCommunicator.h"
#include <QDebug>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QSettings>
#include <QUrlQuery>

#include "Core.h" // For Core::Reports namespace

namespace Core::Reports {

LLMCommunicator::LLMCommunicator(QObject *parent) : ILLMCommunicator(parent) {
  m_networkManager = new QNetworkAccessManager(this);
  m_userId = 0; // 默认用户ID
  connect(m_networkManager, &QNetworkAccessManager::finished, this,
          &LLMCommunicator::onReplyFinished);
}
LLMCommunicator::~LLMCommunicator() {}

void LLMCommunicator::setUserId(int userId) { m_userId = userId; }

void LLMCommunicator::configure(const QString &apiKey, const QString &apiUrl,
                                const QString &modelName) {
  m_apiKey = apiKey;
  m_apiUrl = apiUrl;
  m_modelName = modelName; // e.g. "gpt-3.5-turbo"
}

QString LLMCommunicator::buildPrompt(const CalculatedMetrics &metrics,
                                     const AggregatedData &context) {
  // (This is where the detailed prompt from section 5's example would be
  // constructed dynamically)
  QString prompt =
      QString("你是一位乐于助人的学习教练。请分析以下学生的学习数据：\n"
              "任务完成率：%1%%\n"
              "总学习时长：%2 分钟。\n"
              "各科目时间分配（百分比）：\n")
          .arg(QString::number(metrics.taskCompletionRate, 'f', 1))
          .arg(metrics.totalTimeSpentMinutes);

  for (auto it = metrics.timeAllocationBySubjectPercentage.constBegin();
       it != metrics.timeAllocationBySubjectPercentage.constEnd(); ++it) {
    prompt += QString("- %1: %2%\n")
                  .arg(it.key())
                  .arg(QString::number(it.value(), 'f', 1));
  }
  prompt += "Task Efficiency:\n";
  for (auto it = metrics.taskEfficiencySummary.constBegin();
       it != metrics.taskEfficiencySummary.constEnd(); ++it) {
    prompt += QString("- %1: %2\n").arg(it.key()).arg(it.value());
  }
  // Add more context from AggregatedData.relevantTasks or relevantTimeLogs if
  // desired, summarized. For example, number of tasks completed, most
  // time-consuming tasks etc. Make sure not to exceed token limits.

  prompt +=
      "\n请根据上述数据，生成：\n"
      "1. 一段简要的整体学习情况总结；\n"
      "2. 主要的积极表现或成就；\n"
      "3. 需要改进的地方或可能存在的挑战；\n"
      "4. 2-3条具体可执行的建议。\n"
      // "5. 不要包含```json等代码标识符\n"
      "请将你的回复格式化为一个JSON对象，包含以下字段：\"summary\"（总结，字符"
      "串）、\"positives\"（积极表现，字符串列表）、\"improvements\"（改进建议"
      "，字符串列表）、\"suggestions\"（可执行建议，字符串列表）。";
  return prompt;
}

void LLMCommunicator::requestInsights(const CalculatedMetrics &metrics,
                                      const AggregatedData &contextForLLM,
                                      int requestId) {
  if (m_apiKey.isEmpty() || m_apiUrl.isEmpty()) {
    // qWarning() << "LLM API key or URL not configured.";
    // emit insightsError(requestId, "LLM API not configured.");
    // return;
    QSettings settings("data/set.ini", QSettings::IniFormat);
    bool ok;

    // 根据当前 API URL 判断使用的是哪个模型
    QString modelSection = "llm_Qwen"; // 默认
    QString modelName = "Qwen";
    if (m_apiUrl.contains("googleapis.com")) {
      modelSection = "llm_Gemini";
      modelName = "Gemini";
    } else if (m_apiUrl.contains("aliyuncs.com")) {
      modelSection = "llm_Qwen";
      modelName = "Qwen";
    }

    // 基于用户ID构造配置键名
    QString userPrefix = QString("user_%1_").arg(m_userId);
    QString configKey =
        QString("%1%2/apiKey").arg(userPrefix).arg(modelSection);

    QString apiKey = QInputDialog::getText(
        nullptr, tr("LLM API Key"),
        tr("Please enter your %1 API Key:").arg(modelName), QLineEdit::Password,
        "", &ok);
    if (ok && !apiKey.isEmpty()) {
      settings.setValue(configKey, apiKey);
      settings.sync();   // 确保立即写入文件
      m_apiKey = apiKey; // 更新当前实例的 API Key
    } else {
      QMessageBox::warning(
          nullptr, tr("LLM Configuration"),
          tr("LLM API Key not set. LLM-based analysis will be unavailable."));
      emit insightsError(requestId, "LLM API Key not configured.");
      return;
    }
  }

  QString prompt = buildPrompt(metrics, contextForLLM);
  qDebug() << "LLM Prompt for request ID" << requestId << ":" << prompt;

  // qDebug() << m_apiUrl;
  // Prepare the network request
  QNetworkRequest request(m_apiUrl);

  if (m_apiUrl.contains("googleapis.com")) {
    QString new_Url =
        m_apiUrl.arg(m_modelName).arg(m_apiKey); // Example for Gemini
    request = QNetworkRequest(new_Url);
  } else if (m_apiUrl.contains("aliyuncs.com")) {
    request.setRawHeader("Authorization",
                         QByteArray("Bearer ") + m_apiKey.toUtf8());
  }
  // QString new_Url = m_apiUrl.arg(m_modelName).arg(m_apiKey); // Example for
  // Gemini QNetworkRequest request(new_Url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  // API Key handling depends on the specific LLM API
  // For OpenAI (and many others):
  // request.setRawHeader("Authorization", QByteArray("Bearer ") +
  // m_apiKey.toUtf8());

  // For Gemini (key in URL or specific header as per their docs, might not use
  // Bearer) If key is in URL, m_apiUrl should already include it during
  // configure() if (m_apiUrl.contains("googleapis.com")) { /* Gemini specific
  // headers if any */ }

  QJsonObject requestBody;
  // Structure depends on the API (OpenAI Chat Completion vs Gemini etc.)
  if (m_apiUrl.contains("openai.com")) { // Example for OpenAI
    requestBody["model"] =
        m_modelName.isEmpty() ? "gpt-3.5-turbo" : m_modelName;
    QJsonArray messages;
    QJsonObject message;
    message["role"] = "user";
    message["content"] = prompt;
    messages.append(message);
    requestBody["messages"] = messages;
    requestBody["temperature"] = 0.7; // Adjust creativity
    // requestBody["max_tokens"] = 500; // Optional
  } else if (m_apiUrl.contains("googleapis.com")) { // Example for Gemini Pro
    QJsonObject content;
    QJsonArray parts;
    QJsonObject part;
    part["text"] = prompt;
    parts.append(part);
    content["parts"] = parts;
    QJsonArray contentsArray;
    contentsArray.append(content);
    requestBody["contents"] = contentsArray;
    // Add generationConfig if needed (temperature, maxOutputTokens, etc.)
  } else if (m_apiUrl.contains("aliyuncs.com")) {
    qDebug() << "IN THE QWEN MODE - Using OpenAI compatible format";
    requestBody["model"] = m_modelName.isEmpty() ? "qwen-plus" : m_modelName;
    QJsonArray messages;
    QJsonObject message;
    message["role"] = "user";
    message["content"] = prompt;
    messages.append(message);
    requestBody["messages"] = messages;
    requestBody["temperature"] = 0.7;
  } else {
    qWarning() << "Unsupported LLM API URL structure for request body:"
               << m_apiUrl;
    // Default to a simple text prompt if API structure is unknown
    requestBody["prompt"] = prompt;
  }

  QNetworkReply *reply =
      m_networkManager->post(request, QJsonDocument(requestBody).toJson());
  m_pendingRequests[reply] = requestId;
  qDebug() << "LLM request sent for ID:" << requestId;
}

void LLMCommunicator::onReplyFinished(QNetworkReply *reply) {
  int requestId = m_pendingRequests.take(reply);
  LLMInsights insights;
  insights.analysisSuccessfullyGenerated = false;

  if (reply->error() == QNetworkReply::NoError) {
    QByteArray responseData = reply->readAll();

    // Remove leading/trailing ```json and ```
    QByteArray cleanData = responseData.trimmed();
    // QByteArray prefix = "```json";
    // QByteArray suffix = "```";
    // if (cleanData.startsWith(prefix)) {
    //     cleanData = cleanData.mid(prefix.size()).trimmed();
    // }
    // if (cleanData.endsWith(suffix)) {
    //     cleanData.chop(suffix.size());
    //     cleanData = cleanData.trimmed();
    // }
    // If responseData contains a ```json ... ``` code block, extract only the
    // code block content int startIdx = cleanData.indexOf("```json"); if
    // (startIdx != -1) {
    //     startIdx += 9; // length of "```json"
    //     int endIdx = cleanData.indexOf("```", startIdx);
    //     if (endIdx != -1) {
    //         cleanData = cleanData.mid(startIdx, endIdx - startIdx -
    //         2).trimmed();
    //     }
    // }
    // responseData = cleanData;

    qDebug() << "LLM Response for request ID" << requestId << ":"
             << QString::fromUtf8(responseData);
    insights.rawLLMResponse = QString::fromUtf8(responseData);
    bool parseSuccess = false;
    insights = parseResponse(
        responseData,
        parseSuccess); // Implement this based on LLM's JSON structure
    insights.analysisSuccessfullyGenerated = parseSuccess;
    if (parseSuccess) {
      emit insightsReady(requestId, insights);
    } else {
      insights.summary = "Error parsing LLM response.";
      emit insightsError(requestId, "Error parsing LLM response.");
    }
  } else {
    QString errorMsg =
        QString("LLM API Error: %1. HTTP Status: %2. Details: %3")
            .arg(reply->errorString())
            .arg(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute)
                     .toInt())
            .arg(QString::fromUtf8(reply->readAll())); // Read error body
    qWarning() << errorMsg;
    insights.summary = tr("无法从LLM获取分析: %1").arg(reply->errorString());
    emit insightsError(requestId, errorMsg);
  }
  reply->deleteLater();
}

LLMInsights LLMCommunicator::parseResponse(const QByteArray &responseData,
                                           bool &success) {
  // This function is highly dependent on the specific LLM API's response
  // format. Example for the JSON structure requested in the prompt: {
  // "summary": "...", "positives": ["..."], "improvements": ["..."],
  // "suggestions": ["..."] }
  LLMInsights insights;
  success = false;
  QJsonDocument doc = QJsonDocument::fromJson(responseData);

  qDebug() << "SUCCESS IN PARSE!!!!!!!!!!!!!!!";

  if (doc.isNull() || !doc.isObject()) {
    qWarning() << "LLM response is not a valid JSON object.";
    return insights;
  }
  QJsonObject obj = doc.object();

  // OpenAI specific parsing (chat completions)
  if (obj.contains("choices") && obj["choices"].isArray()) {
    qDebug() << "OPENAI MODE";
    QJsonArray choices = obj["choices"].toArray();
    if (!choices.isEmpty() && choices.first().isObject()) {
      QJsonObject firstChoice = choices.first().toObject();
      if (firstChoice.contains("message") &&
          firstChoice["message"].isObject()) {
        QJsonObject messageObj = firstChoice["message"].toObject();
        if (messageObj.contains("content") &&
            messageObj["content"].isString()) {
          QString contentStr = messageObj["content"].toString();
          // The contentStr is expected to be a JSON string itself
          // 去除头尾
          QString cleanData = contentStr.trimmed();
          QByteArray prefix = "```json";
          QByteArray suffix = "```";
          if (cleanData.startsWith(prefix)) {
            cleanData = cleanData.mid(prefix.size()).trimmed();
          }
          if (cleanData.endsWith(suffix)) {
            cleanData.chop(suffix.size());
            cleanData = cleanData.trimmed();
          }
          contentStr = cleanData;

          QJsonDocument contentDoc =
              QJsonDocument::fromJson(contentStr.toUtf8());
          if (contentDoc.isObject()) {
            obj =
                contentDoc.object(); // Re-assign obj to the actual content JSON
          } else {
            qWarning() << "LLM message content is not a JSON object string: "
                       << contentStr;
            // Fallback: try to use contentStr directly if it's not JSON but
            // seems like a summary
            insights.summary = contentStr;
            // No detailed parsing possible if the inner content isn't the
            // expected JSON
            success =
                !insights.summary.trimmed()
                     .isEmpty(); // Consider it a success if we got some text
            return insights;
          }
        } else {
          qWarning() << "LLM message object does not contain string 'content'.";
          return insights;
        }
      } else {
        qWarning() << "LLM first choice does not contain object 'message'.";
        return insights;
      }
    } else {
      qWarning()
          << "LLM choices array is empty or first element is not an object.";
      return insights;
    }
  } else if (obj.contains("candidates") &&
             obj["candidates"].isArray()) { // Gemini specific parsing
    QJsonArray candidates = obj["candidates"].toArray();
    if (!candidates.isEmpty() && candidates.first().isObject()) {
      QJsonObject firstCandidate = candidates.first().toObject();
      if (firstCandidate.contains("content") &&
          firstCandidate["content"].isObject()) {
        QJsonObject contentObj = firstCandidate["content"].toObject();
        if (contentObj.contains("parts") && contentObj["parts"].isArray()) {
          QJsonArray parts = contentObj["parts"].toArray();
          if (!parts.isEmpty() && parts.first().isObject() &&
              parts.first().toObject().contains("text")) {
            QString textContent = parts.first().toObject()["text"].toString();
            // Remove leading/trailing ```json and ```
            QString cleanData = textContent.trimmed();
            QByteArray prefix = "```json";
            QByteArray suffix = "```";
            if (cleanData.startsWith(prefix)) {
              cleanData = cleanData.mid(prefix.size()).trimmed();
            }
            if (cleanData.endsWith(suffix)) {
              cleanData.chop(suffix.size());
              cleanData = cleanData.trimmed();
            }
            textContent = cleanData;

            QJsonDocument contentDoc =
                QJsonDocument::fromJson(textContent.toUtf8());
            if (contentDoc.isObject()) {
              obj = contentDoc
                        .object(); // Re-assign obj to the actual content JSON
            } else {
              qWarning()
                  << "LLM (Gemini) part text is not a JSON object string: "
                  << textContent;
              insights.summary = textContent;
              success = !insights.summary.trimmed().isEmpty();
              return insights;
            }
          } else {
            qWarning() << "LLM (Gemini) content parts missing or invalid.";
            return insights;
          }
        } else {
          qWarning() << "LLM (Gemini) content object has no 'parts' array.";
          return insights;
        }
      } else {
        qWarning() << "LLM (Gemini) first candidate has no 'content' object.";
        return insights;
      }
    } else {
      qWarning() << "LLM (Gemini) candidates array is empty or invalid.";
      return insights;
    }
  } else if (obj.contains("output") &&
             obj["output"].isObject()) { // Qwen API typical response
    // QString outputStr = obj["output"].toString();
    qDebug() << "Qwen API Mode";
    QJsonObject output = obj["output"].toObject();
    QJsonArray choices = output["choices"].toArray();
    // Qwen's output is usually a JSON string, so parse it
    if (!choices.isEmpty() && choices.first().isObject()) {
      QJsonObject firstChoice = choices.first().toObject();
      if (firstChoice.contains("message") &&
          firstChoice["message"].isObject()) {
        QJsonObject messageObj = firstChoice["message"].toObject();
        if (messageObj.contains("content") &&
            messageObj["content"].isString()) {
          QString contentStr = messageObj["content"].toString();
          // 去除头尾
          QString cleanData = contentStr.trimmed();
          QByteArray prefix = "```json";
          QByteArray suffix = "```";
          if (cleanData.startsWith(prefix)) {
            cleanData = cleanData.mid(prefix.size()).trimmed();
          }
          if (cleanData.endsWith(suffix)) {
            cleanData.chop(suffix.size());
            cleanData = cleanData.trimmed();
          }
          contentStr = cleanData;
          // The contentStr is expected to be a JSON string itself
          QJsonDocument contentDoc =
              QJsonDocument::fromJson(contentStr.toUtf8());
          if (contentDoc.isObject()) {
            obj =
                contentDoc.object(); // Re-assign obj to the actual content JSON
          } else {
            qWarning() << "LLM message content is not a JSON object string: "
                       << contentStr;
            // Fallback: try to use contentStr directly if it's not JSON but
            // seems like a summary
            insights.summary = contentStr;
            // No detailed parsing possible if the inner content isn't the
            // expected JSON
            success =
                !insights.summary.trimmed()
                     .isEmpty(); // Consider it a success if we got some text
            return insights;
          }
        } else {
          qWarning() << "LLM message object does not contain string 'content'.";
          return insights;
        }
      } else {
        qWarning() << "LLM first choice does not contain object 'message'.";
        return insights;
      }
    } else {
      qWarning()
          << "LLM choices array is empty or first element is not an object.";
      return insights;
    }
  }
  // Generic parsing based on the prompt's requested JSON structure
  if (obj.contains("summary") && obj["summary"].isString()) {
    insights.summary = obj["summary"].toString();
    success = true; // Basic success if summary is found
  }
  if (obj.contains("positives") && obj["positives"].isArray()) {
    for (const QJsonValue &val : obj["positives"].toArray()) {
      if (val.isString())
        insights.positiveObservations.append(val.toString());
    }
  }
  if (obj.contains("improvements") && obj["improvements"].isArray()) {
    for (const QJsonValue &val : obj["improvements"].toArray()) {
      if (val.isString())
        insights.areasForImprovement.append(val.toString());
    }
  }
  if (obj.contains("suggestions") && obj["suggestions"].isArray()) {
    for (const QJsonValue &val : obj["suggestions"].toArray()) {
      if (val.isString())
        insights.actionableSuggestions.append(val.toString());
    }
  }
  if (!success && !obj.isEmpty()) { // If specific fields not found but got some
                                    // JSON, try to extract text
    qWarning() << "LLM response JSON did not match expected structure. Trying "
                  "generic extraction.";
    // Attempt to extract some meaningful text if parsing fails on specific
    // fields
    QString fallbackText;
    for (const QString &key : obj.keys()) {
      if (obj[key].isString())
        fallbackText += obj[key].toString() + "\n";
      // Could also iterate arrays of strings, etc.
    }
    if (!fallbackText.isEmpty()) {
      insights.summary = tr("LLM Response (unexpected format):\n%1")
                             .arg(fallbackText.trimmed());
      success = true;
    }
  }

  return insights;
}

} // namespace Core::Reports