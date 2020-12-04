#include "currentconfig.h"
#include "ui_currentconfig.h"
#include "common_defines.h"

CurrentConfig::CurrentConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurrentConfig)
{
    ui->setupUi(this);

    m_defaultLabelStyle = "";
    m_maxButtonsWarning = false;
    m_axisSources = 0;
    m_buttonsFromAxes = 0;
    m_buttonsFromShiftRegs = 0;
    m_singleButtons = 0;
    m_rowsOfButtons = 0;
    m_columnsOfButtons = 0;
    m_singleLed = 0;
    m_rowsOfLed = 0;
    m_columnsOfLed = 0;
}

CurrentConfig::~CurrentConfig()
{
    delete ui;
}


void CurrentConfig::setConfig(int type, int changedHowMuch)
{
    if (type == AXIS_SOURCE){      //int source_enum, bool is_add      axesSourceChanged
        m_axisSources+=changedHowMuch;
        ui->label_AxisSources->setNum(m_axisSources);
//        if (changedHowMuch > 0){
//            emit axesSourceChanged(pinNumber - 1, true);
//        } else {
//            emit axesSourceChanged(pinNumber - 1, false);
//        }
    }
    else if (type == SINGLE_BUTTON){
        m_singleButtons+=changedHowMuch;
        ui->label_SingleButtons->setNum(m_singleButtons);
        ui->label_TotalButtons->setNum(m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons + (m_columnsOfButtons * m_rowsOfButtons));
        totalButtonsChanged(m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons + (m_columnsOfButtons * m_rowsOfButtons));
    }
    else if (type == ROW_OF_BUTTONS){
        m_rowsOfButtons+=changedHowMuch;
        ui->label_RowsOfButtons->setNum(m_rowsOfButtons);
        ui->label_TotalButtons->setNum(m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons + (m_columnsOfButtons * m_rowsOfButtons));
        totalButtonsChanged(m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons + (m_columnsOfButtons * m_rowsOfButtons));
    }
    else if (type == COLUMN_OF_BUTTONS){
        m_columnsOfButtons+=changedHowMuch;
        ui->label_ColumnsOfButtons->setNum(m_columnsOfButtons);
        ui->label_TotalButtons->setNum(m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons + (m_columnsOfButtons * m_rowsOfButtons));
        totalButtonsChanged(m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons + (m_columnsOfButtons * m_rowsOfButtons));
    }
    else if (type == SINGLE_LED){
        m_singleLed+=changedHowMuch;
        ui->label_TotalLEDs->setNum(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
        emit totalLEDsValueChanged(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
    }
    else if (type == ROW_OF_LED){
        m_rowsOfLed+=changedHowMuch;
        ui->label_TotalLEDs->setNum(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
        emit totalLEDsValueChanged(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
    }
    else if (type == COLUMN_OF_LED){
        m_columnsOfLed+=changedHowMuch;
        ui->label_TotalLEDs->setNum(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
        emit totalLEDsValueChanged(m_singleLed + (m_rowsOfLed * m_columnsOfLed));
    }
}

void CurrentConfig::a2bCountChanged(int count)
{
    m_buttonsFromAxes = count;
    ui->label_ButtonFromAxes->setNum(m_buttonsFromAxes);
    int totalButtons = m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons + (m_columnsOfButtons * m_rowsOfButtons);
    ui->label_TotalButtons->setNum(totalButtons);
    totalButtonsChanged(totalButtons);
}

void CurrentConfig::shiftRegButtonsCountChanged(int count)
{
    m_buttonsFromShiftRegs = count;
    ui->label_ButtonsFromShiftRegs->setNum(m_buttonsFromShiftRegs);
    int totalButtons = m_buttonsFromShiftRegs + m_buttonsFromAxes + m_singleButtons + (m_columnsOfButtons * m_rowsOfButtons);
    ui->label_TotalButtons->setNum(totalButtons);
    totalButtonsChanged(totalButtons);
}

void CurrentConfig::totalButtonsChanged(int count)
{
    if (count > MAX_BUTTONS_NUM){
        m_defaultLabelStyle = ui->label_TotalButtons->styleSheet();
        //ui->label_TotalButtons->setStyleSheet(default_label_style_ + "background-color: rgb(200, 0, 0);");
        ui->text_TotalButtons->setStyleSheet(m_defaultLabelStyle + QStringLiteral("background-color: rgb(200, 0, 0);"));
        m_maxButtonsWarning = true;
    } else if (m_maxButtonsWarning == true){   // && count <= MAX_BUTTONS_NUM
        // а не будет ли тут проблемы, если кнопка активна и в это время изменился стиль приложения
        // то default_label_style_ будет же со старым стилем?
        ui->text_TotalButtons->setStyleSheet(m_defaultLabelStyle);
        m_maxButtonsWarning = false;
    }
}
