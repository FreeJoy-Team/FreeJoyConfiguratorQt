#include "buttonlogical.h"
#include "ui_buttonlogical.h"

#include "widgets/debugwindow.h"
#include "converter.h"

int ButtonLogical::m_currentFocus = -1;
bool ButtonLogical::m_autoPhysButEnabled = false;

ButtonLogical::ButtonLogical(int buttonNumber, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ButtonLogical)
{
    ui->setupUi(this);
    m_buttonNumber = buttonNumber;
    m_functionPrevIndex = 0;
    m_currentState = false;
    ui->label_LogicalButtonNumber->setNum(m_buttonNumber + 1);
    ui->spinBox_PhysicalButtonNumber->installEventFilter(this);
}

ButtonLogical::~ButtonLogical()
{
    delete ui;
}

void ButtonLogical::retranslateUi()
{
    ui->retranslateUi(this);
}

void ButtonLogical::initialization()
{
    // add gui text
    m_logicFunc_enumIndex.reserve(m_logicFunctionList.size());
    for (int i = 0; i < m_logicFunctionList.size(); i++) {
//        // Encoder only for first 30 buttons  // if uncomment DONT FORGET EDIT MainWindow::oldConfigHandler() !!!!!!!!
//        if (m_buttonNumber > 29 &&
//                (m_logicFunctionList[i].deviceEnumIndex == ENCODER_INPUT_A ||
//                 m_logicFunctionList[i].deviceEnumIndex == ENCODER_INPUT_B)) {
//            continue;
//        }
        ui->comboBox_ButtonFunction->addItem(m_logicFunctionList[i].guiName);
        m_logicFunc_enumIndex.push_back(m_logicFunctionList[i].deviceEnumIndex);
    }
    for (int i = 0; i < SHIFT_COUNT; i++) {
        ui->comboBox_ShiftIndex->addItem(m_shiftList[i].guiName);
    }
    for (int i = 0; i < TIMER_COUNT; i++) {
        ui->comboBox_DelayTimerIndex->addItem(m_timerList[i].guiName);
        ui->comboBox_PressTimerIndex->addItem(m_timerList[i].guiName);
    }

    connect(ui->comboBox_ButtonFunction, SIGNAL(currentIndexChanged(int)),
            this, SLOT(functionTypeChanged(int)));
    connect(ui->spinBox_PhysicalButtonNumber, SIGNAL(valueChanged(int)),
            this, SLOT(editingOnOff(int)));
}

void ButtonLogical::setMaxPhysButtons(int maxPhysButtons)
{
    ui->spinBox_PhysicalButtonNumber->setMaximum(maxPhysButtons);
}

void ButtonLogical::setSpinBoxOnOff(int maxPhysButtons)
{
    if (maxPhysButtons > 0) {
        ui->spinBox_PhysicalButtonNumber->setEnabled(true);
    } else {
        ui->spinBox_PhysicalButtonNumber->setEnabled(false);
    }
}

void ButtonLogical::functionTypeChanged(int index)
{
    emit functionIndexChanged(index, m_functionPrevIndex, m_buttonNumber);
    m_functionPrevIndex = index;
}

void ButtonLogical::editingOnOff(int value)
{
    if (value > 0 && ui->spinBox_PhysicalButtonNumber->isEnabled()) {
        ui->checkBox_IsInvert->setEnabled(true);
        ui->checkBox_IsDisable->setEnabled(true);
        ui->comboBox_ButtonFunction->setEnabled(true);
        ui->comboBox_ShiftIndex->setEnabled(true);
        ui->comboBox_DelayTimerIndex->setEnabled(true);
        ui->comboBox_PressTimerIndex->setEnabled(true);
    } else {
        ui->checkBox_IsInvert->setEnabled(false);
        ui->checkBox_IsDisable->setEnabled(false);
        ui->comboBox_ButtonFunction->setEnabled(false);
        ui->comboBox_ShiftIndex->setEnabled(false);
        ui->comboBox_DelayTimerIndex->setEnabled(false);
        ui->comboBox_PressTimerIndex->setEnabled(false);
    }
}

void ButtonLogical::setButtonState(bool state)
{
    static QPalette default_palette;    ///////////////////////////////////////////////// ????????????????????????????
    static QString default_style;    ///////////////////////////////////////////////// ????????????????????????????

    if (state != m_currentState) {
        setAutoFillBackground(true);

        if (state) {
            default_palette = palette();
            default_style = ui->label_LogicalButtonNumber->styleSheet(); // default_style ="" бесполезно

            setPalette(QPalette(QPalette::Window, QColor(0, 128, 0)));
            ui->label_LogicalButtonNumber->setStyleSheet(default_style
                                                         + QStringLiteral("background-color: rgb(0, 128, 0);"));

            if (gEnv.pDebugWindow) {
                gEnv.pDebugWindow->logicalButtonState(m_buttonNumber + 1, true);
            }
        } else {
            setPalette(window()->palette());
            ui->label_LogicalButtonNumber->setStyleSheet(default_style);

            if (gEnv.pDebugWindow) {
                gEnv.pDebugWindow->logicalButtonState(m_buttonNumber + 1, false);
            }
        }
        m_currentState = state;
    }
}

void ButtonLogical::setLogicButton(int buttonNumber)
{
    ui->spinBox_PhysicalButtonNumber->setValue(buttonNumber + 1); // +1 !!!!
}

int ButtonLogical::currentFocus() const
{
    return m_currentFocus;
}

void ButtonLogical::setAutoPhysBut(bool enabled)
{
    m_autoPhysButEnabled = enabled;
}

bool ButtonLogical::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)
    if (m_autoPhysButEnabled) {
        if (event->type() == QEvent::FocusIn) {
            ui->spinBox_PhysicalButtonNumber->setStyleSheet("background-color: rgba(0, 120, 215, 200); color: rgb(255, 255, 255)");
            m_currentFocus = m_buttonNumber;
        } else if (event->type() == QEvent::FocusOut){
            ui->spinBox_PhysicalButtonNumber->setStyleSheet("");
            m_currentFocus = -1;
        }
    }
    return false;
}

void ButtonLogical::readFromConfig()
{
    button_t *button = &gEnv.pDeviceConfig->config.buttons[m_buttonNumber];
    // physical
    ui->spinBox_PhysicalButtonNumber->setValue(button->physical_num + 1); // +1 !!!!
    // isDisable
    ui->checkBox_IsDisable->setChecked(button->is_disabled);
    // isInvert
    ui->checkBox_IsInvert->setChecked(button->is_inverted);

    // logical button function
    ui->comboBox_ButtonFunction->setCurrentIndex(Converter::EnumToIndex(button->type, m_logicFunc_enumIndex));
    // shift
    for (int i = 0; i < SHIFT_COUNT; i++) {
        if (button->shift_modificator == m_shiftList[i].deviceEnumIndex) {
            ui->comboBox_ShiftIndex->setCurrentIndex(i);
            break;
        }
    }
    // delay timer
    for (int i = 0; i < TIMER_COUNT; i++) {
        if (button->delay_timer == m_timerList[i].deviceEnumIndex) {
            ui->comboBox_DelayTimerIndex->setCurrentIndex(i);
            break;
        }
    }
    // toggle timer
    for (int i = 0; i < TIMER_COUNT; i++) {
        if (button->press_timer == m_timerList[i].deviceEnumIndex) {
            ui->comboBox_PressTimerIndex->setCurrentIndex(i);
            break;
        }
    }
}

void ButtonLogical::writeToConfig()
{
    button_t *button = &gEnv.pDeviceConfig->config.buttons[m_buttonNumber];

    button->physical_num = ui->spinBox_PhysicalButtonNumber->value() - 1; // -1 !!!!
    button->is_disabled = ui->checkBox_IsDisable->isChecked();
    button->is_inverted = ui->checkBox_IsInvert->isChecked();

    button->type = m_logicFunc_enumIndex[ui->comboBox_ButtonFunction->currentIndex()];
    button->shift_modificator = m_shiftList[ui->comboBox_ShiftIndex->currentIndex()].deviceEnumIndex;
    button->delay_timer = m_timerList[ui->comboBox_DelayTimerIndex->currentIndex()].deviceEnumIndex;
    button->press_timer = m_timerList[ui->comboBox_PressTimerIndex->currentIndex()].deviceEnumIndex;
}
