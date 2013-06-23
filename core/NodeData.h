#ifndef NODEDATA_H
#define NODEDATA_H

#include <QString>
#include <QList>
#include <QMap>
#include <QVariant>

struct NodeData {
    long id;
    QString name;
    QList<long> relations;
    QMap<QString, QMap<QString, QVariant> > attributes;

    QString attributesAsText() const {

      Json::Value jsonValue;
      foreach (QString key, attributes.keys()) {
        QMap<QString, QVariant> value = attributes.value(key);
        Json::Value innerJsonValue;
        foreach (QString innerKey, value.keys()) {
          QVariant innerValue = value.value(innerKey);
          if (innerValue.type() == QVariant::StringList) {
            Json::Value array = Json::Value(Json::arrayValue);
            foreach(QString string, innerValue.toStringList()) {
              array.append(Json::Value(string.toStdString()));
            }
            innerJsonValue[innerKey.toStdString()] = array;
          }
          else {
            innerJsonValue[innerKey.toStdString()] = Json::Value(innerValue.toString().toStdString());
          }
        }
        jsonValue[key.toStdString()] = innerJsonValue;
      }

      return QString::fromStdString(jsonValue.toStyledString());
    }

    Json::Value attributesAsJson() const {

      Json::Value jsonValue;
      foreach (QString key, attributes.keys()) {
        QMap<QString, QVariant> value = attributes.value(key);
        Json::Value innerJsonValue;
        foreach (QString innerKey, value.keys()) {
          QVariant innerValue = value.value(innerKey);
          if (innerValue.type() == QVariant::StringList) {
            Json::Value array = Json::Value(Json::arrayValue);
            foreach(QString string, innerValue.toStringList()) {
              array.append(Json::Value(string.toStdString()));
            }
            innerJsonValue[innerKey.toStdString()] = array;
          }
          else {
            innerJsonValue[innerKey.toStdString()] = Json::Value(innerValue.toString().toStdString());
          }
        }
        jsonValue[key.toStdString()] = innerJsonValue;
      }

      return jsonValue;
    }

    void setAttributesFromText(const QString &text) {

      Json::Reader reader;
      Json::Value jsonValue;
      bool ok = reader.parse(text.toStdString(), jsonValue);

      if (ok) {
        attributes.clear();
        for (int i = 0; i < jsonValue.size(); ++i) {
          QString key = QString::fromStdString(jsonValue.getMemberNames().at(i));
          Json::Value value = jsonValue[jsonValue.getMemberNames().at(i)];

          qDebug() << QString::fromStdString(value.toStyledString());

          QMap<QString, QVariant> innerValue;
          for (int j = 0; j < value.size(); ++j) {
            QString innerKey = QString::fromStdString(value.getMemberNames().at(i));
            if (value[value.getMemberNames().at(i)].isArray()) {
              QStringList list;
              for (int k = 0; k < value[value.getMemberNames().at(i)].size(); ++k) {
                Json::Value arrayItem = value[value.getMemberNames().at(i)][k];
                list.append(QString::fromStdString(arrayItem.asString()));
              }
            }
            else {
              QVariant innerValueValue = QVariant(QString::fromStdString(value[value.getMemberNames().at(i)].asString()));
              innerValue.insert(innerKey, innerValueValue);
            }
          }
          attributes.insert(key, innerValue);
        }
      }
    }
};

#endif // NODEDATA_H
