#ifndef ONTOLOGYGRAPHELEMENT_H
#define ONTOLOGYGRAPHELEMENT_H

#include <QString>

class OntologyGraphElement {
  protected:
    long m_id;
    QString m_name;

  public:
    OntologyGraphElement();

    void setId(long id);
    long id() const;

    void setName(const QString &name);
    QString name() const;
};

#endif // ONTOLOGYGRAPHELEMENT_H
