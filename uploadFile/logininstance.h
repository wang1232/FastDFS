#ifndef LOGININSTANCE_H
#define LOGININSTANCE_H
# include<QString>

// 单例模式——饿汉模式
class LoginInstance
{
public:
    static LoginInstance* getInstance();
    void setUserName(QString name);
    QString getName();
private:
    LoginInstance();
    LoginInstance(const LoginInstance& t);
    static LoginInstance m_login;
    // 存储的数据
    QString m_user;
    QString m_passwd;
    QString m_ip;
    QString m_port;
    QString m_token;

};

#endif // LOGININSTANCE_H
