#include "metermodel.h"

MeterModel::MeterModel(QObject* _parent)
    : QObject(_parent)
{
}

int MeterModel::denominatorModel() const
{
    return m_denominatorModel;
}

void MeterModel::setDenominatorModel(int newDenominatorModel)
{
    if (m_denominatorModel == newDenominatorModel)
        return;
    m_denominatorModel = newDenominatorModel;
    emit denominatorModelChanged();
}

int MeterModel::numeratorModel() const
{
    return m_numeratorModel;
}

void MeterModel::setNumeratorModel(int newNumeratorModel)
{

    if (m_numeratorModel == newNumeratorModel)
        return;

    m_numeratorModel = newNumeratorModel;
    emit numeratorModelChanged();
}
