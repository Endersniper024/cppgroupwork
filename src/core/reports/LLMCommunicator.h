#ifndef LLMCOMMUNICATOR_H
#define LLMCOMMUNICATOR_H

#include "ILLMCommunicator.h"
#include <QWidget>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>


#include "Core.h" //

namespace Core::Reports {

class LLMCommunicator : public ILLMCommunicator {
  Q_OBJECT
public:
  explicit LLMCommunicator(QObject *parent = nullptr);
  ~LLMCommunicator();

  void configure(const QString &apiKey, const QString &apiUrl,
                 const QString &modelName = "") override;
  void setUserId(int userId);            // 设置当前用户ID
  void setParentWidget(QWidget *parent); // 设置父窗口
  void requestInsights(const CalculatedMetrics &metrics,
                       const AggregatedData &contextForLLM,
                       int requestId = 0) override;

private slots:
  void onReplyFinished(QNetworkReply *reply);

private:
  QString m_apiKey;
  QString m_apiUrl;
  QString m_modelName;     // e.g., "gpt-3.5-turbo", "gemini-pro"
  int m_userId;            // 当前用户ID
  QWidget *m_parentWidget; // 父窗口指针
  QNetworkAccessManager *m_networkManager;
  QMap<QNetworkReply *, int> m_pendingRequests; // Map reply to requestId

  QString buildPrompt(const CalculatedMetrics &metrics,
                      const AggregatedData &contextForLLM);
  LLMInsights parseResponse(const QByteArray &responseData,
                            bool &success); // Implementation depends on LLM
  bool validateApiKey(const QString &apiKey,
                      const QString &modelName); // 验证API Key格式
  QString
  promptForValidApiKey(const QString &modelName); // 提示输入有效的API Key
};
} // namespace Core::Reports

#endif // LLMCOMMUNICATOR_H