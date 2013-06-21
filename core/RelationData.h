#ifndef RELATIONDATA_H
#define RELATIONDATA_H

#include <QString>
#include <QMap>

struct RelationData {
    long id;
    long sourceNodeId;
    long destinationNodeId;
    QString name;
    QMap<QString, QString> attributes;

    QString attributesAsText() const {

      Json::Value jsonValue;
      foreach (QString key, attributes.keys()) {
        QString value = attributes.value(key);
        jsonValue[key.toStdString()] = Json::Value(value.toStdString());
      }

      return QString::fromStdString(jsonValue.toStyledString());
    }

    Json::Value attributesAsJson() const {

      Json::Value jsonValue;
      foreach (QString key, attributes.keys()) {
        QString value = attributes.value(key);
        jsonValue[key.toStdString()] = Json::Value(value.toStdString());
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
          QString value = QString::fromStdString(jsonValue[jsonValue.getMemberNames().at(i)].asString());
          attributes.insert(key, value);
        }
      }
    }
};

#endif // RELATIONDATA_H
