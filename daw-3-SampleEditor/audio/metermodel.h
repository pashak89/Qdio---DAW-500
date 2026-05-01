#ifndef METERMODEL_H
#define METERMODEL_H

#include <QObject>

class MeterModel : public QObject {
    Q_OBJECT

public:
    MeterModel(QObject* _parent = Q_NULLPTR);
    ~MeterModel() override = default;

    void reset();

    int denominatorModel() const;
    void setDenominatorModel(int newDenominatorModel);

    int numeratorModel() const;
    void setNumeratorModel(int newNumeratorModel);

signals:
    void denominatorModelChanged();
    void numeratorModelChanged();

private:
    int m_numeratorModel;
    int m_denominatorModel;
};

#endif // METERMODEL_H
