#include "currentconfig.h"
#include "ui_currentconfig.h"
#include "common_defines.h"

CurrentConfig::CurrentConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurrentConfig)
{
    ui->setupUi(this);

    m_axisSources = 0;
    m_buttonsFromAxes = 0;
    m_buttonsFromShiftRegs = 0;
    m_singleButtons = 0;
    m_rowsOfButtons = 0;
    m_columnsOfButtons = 0;
    m_singleLed = 0;
    m_rowsOfLed = 0;
    m_columnsOfLed = 0;

    m_limit = false;
}

CurrentConfig::~CurrentConfig()
{
    delete ui;
}


void CurrentConfig::setConfig(int type, int changedHowMuch)
{
    if (type == AXIS_SOURCE){      //int source_enum, bool is_add      axesSourceChanged
        m_axisSources += changedHowMuch;
        ui->label_AxisSources->setNum(m_axisSources);
//        if (changedHowMuch > 0){
//            emit axesSourceChanged(pinNumber - 1, true);
//        } else {
//            emit axesSourceChanged(pinNumber - 1, false);
//        }
    }
    else if (type == SINGLE_BUTTON){
        m_singleButtons += changedHowMuch;
        ui->label_SingleButtons->setNum(m_singleButtons);
        updateButtons();
    }
    else if (type == ROW_OF_BUTTONS){
        m_rowsOfButtons += changedHowMuch;
        ui->label_RowsOfButtons->setNum(m_rowsOfButtons);
        ui->label_ButtonsFromMatrix->setNum(m_columnsOfButtons * m_rowsOfButtons);
        updateButtons();
    }
    else if (type == COLUMN_OF_BUTTONS){
        m_columnsOfButtons += changedHowMuch;
        ui->label_ColumnsOfButtons->setNum(m_columnsOfButtons);
        ui->label_ButtonsFromMatrix->setNum(m_columnsOfButtons * m_rowsOfButtons);
        updateButtons();
    }
    else if (type == SINGLE_LED){
        m_singleLed += changedHowMuch;
        ui->label_TotalLEDs->setNum(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
        totalLEDsChanged(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
    }
    else if (type == ROW_OF_LED){
        m_rowsOfLed += changedHowMuch;
        ui->label_TotalLEDs->setNum(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
        totalLEDsChanged(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
    }
    else if (type == COLUMN_OF_LED){
        m_columnsOfLed += changedHowMuch;
        ui->label_TotalLEDs->setNum(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
        totalLEDsChanged(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
    }
}

void CurrentConfig::a2bCountChanged(int count)
{
    m_buttonsFromAxes = count;
    ui->label_ButtonFromAxes->setNum(m_buttonsFromAxes);
    updateButtons();
}

void CurrentConfig::shiftRegButtonsCountChanged(int count)
{
    m_buttonsFromShiftRegs = count;
    ui->label_ButtonsFromShiftRegs->setNum(m_buttonsFromShiftRegs);
    updateButtons();
}

int CurrentConfig::totalButtons() const
{
    return m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons
            + m_columnsOfButtons * m_rowsOfButtons + m_buttonsFromBcd;
}

void CurrentConfig::updateButtons()
{
    ui->label_TotalButtons->setNum(totalButtons());
    totalButtonsChanged(totalButtons());
}

void CurrentConfig::bcdButtonsCountChanged(int count)
{
    if (count == m_buttonsFromBcd) return;
    m_buttonsFromBcd = count;
    ui->label_BcdButtons->setNum(count);
    updateButtons();
}

void CurrentConfig::updateLimit()
{
    const bool limit = totalButtons() > MAX_BUTTONS_NUM || m_totalLEDs > MAX_LEDS_NUM;
    if (limit != m_limit) {
        m_limit = limit;
        emit limitReached(limit);
    }
}

void CurrentConfig::totalButtonsChanged(int count)
{
    const QString style = count > MAX_BUTTONS_NUM
            ? QStringLiteral("background-color: rgb(200, 0, 0);") : QString();
    ui->label_TotalButtons->setStyleSheet(style);
    ui->text_TotalButtons->setStyleSheet(style);
    updateLimit();
    // The firmware's physical button indices are limited to 0..127.
    emit totalButtonsValueChanged(qMin(count, MAX_BUTTONS_NUM));
}

void CurrentConfig::totalLEDsChanged(int count)
{
    m_totalLEDs = count;
    const QString style = count > MAX_LEDS_NUM
            ? QStringLiteral("background-color: rgb(200, 0, 0);") : QString();
    ui->label_TotalLEDs->setStyleSheet(style);
    ui->text_TotalLEDs->setStyleSheet(style);
    updateLimit();
    emit totalLEDsValueChanged(count);
}

bool CurrentConfig::limitIsReached()
{
    return m_limit;
}
