#include "pincombobox.h"
#include "ui_pincombobox.h"

#define PIN_TYPES_FOR_ALL 16

PinComboBox::PinComboBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PinComboBox)
{
    ui->setupUi(this);

    // во все комбобоксы из pin_list_ в количестве PIN_TYPES_FOR_ALL
    for (int i = 0; i < PIN_TYPES_FOR_ALL; i++) {
        ui->comboBox_PinsType->addItem(pin_list_[i].gui_name);
    }
    //    for (int i = 0 ; i < ui->comboBox_Pin->count() ; ++i)         // align
    //    {
    //        ui->comboBox_Pin->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
    //    }
//    connect(ui->comboBox_PinsType, SIGNAL(currentIndexChanged(int)),
//                this, SLOT(IndexToEnum(int)));
        connect(ui->comboBox_PinsType, SIGNAL(currentIndexChanged(int)),
                    this, SLOT(IndexChanged(int)));

}

PinComboBox::~PinComboBox()
{
    delete ui;
}

void PinComboBox::IndexChanged(int index)       // ?????
{
    index = 0;
    emit valueChanged();
}


int PinComboBox::GetEnumValue()     //????????
{
    for (int i = 0; i < PIN_TYPE_COUNT; ++i) {
        if (ui->comboBox_PinsType->currentText() == pin_list_[i].gui_name)
        {
            return pin_list_[i].device_enum_index;
        }
    }
    return 0;
    //emit valueChanged2(pin_list_index);
}

//! Returns a string to add to ComboBox
QString PinComboBox::EnumToString(int deviceEnum)               // try catch
{
    for (int i = 0; i < PIN_TYPE_COUNT; i++){
        if (pin_list_[i].device_enum_index == deviceEnum){
            return pin_list_[i].gui_name;
        }
    }
    return "Not Found";
}

//! Returns the index to remove from ComboBox
int PinComboBox::EnumToIndex(int deviceEnum)                    // try catch
{
    QString string;
    for (int i = 0; i < PIN_TYPE_COUNT; i++){
        if (pin_list_[i].device_enum_index == deviceEnum){
            string = pin_list_[i].gui_name;
            break;
        }
    }
    return ui->comboBox_PinsType->findText(string);
}

//! Set and remove bla bla bla
void PinComboBox::Initialization_pinA(int pin)
{
    pin_a_ = pin;
    if (pin < 8){
        ui->comboBox_PinsType->addItem(EnumToString(AXIS_ANALOG));
    }
    if (pin == 8 || pin == 9) {
        ui->comboBox_PinsType->addItem(EnumToString(FAST_ENCODER));
    }
}

//! Set and remove bla bla bla
void PinComboBox::Initialization_pinB(int pin)
{
    pin_b_ = pin;
    if (pin == 0 || pin == 1 || pin == 4)
    {
        ui->comboBox_PinsType->addItem(EnumToString(LED_PWM));
    }
    if (pin == 3)
    {
        // remove
        ui->comboBox_PinsType->removeItem(EnumToIndex(TLE5011_CS));
        ui->comboBox_PinsType->removeItem(EnumToIndex(MCP3201_CS));
        ui->comboBox_PinsType->removeItem(EnumToIndex(MCP3202_CS));
        ui->comboBox_PinsType->removeItem(EnumToIndex(MCP3204_CS));
        ui->comboBox_PinsType->removeItem(EnumToIndex(MCP3208_CS));
        ui->comboBox_PinsType->removeItem(EnumToIndex(MLX90393_CS));
        ui->comboBox_PinsType->removeItem(EnumToIndex(SHIFT_REG_LATCH));
        ui->comboBox_PinsType->removeItem(EnumToIndex(SHIFT_REG_DATA));
        // add
        ui->comboBox_PinsType->addItem(EnumToString(SPI_SCK));
    }
    if (pin == 4)
    {
        // add
        ui->comboBox_PinsType->addItem(EnumToString(SPI_MISO));
    }
    if (pin == 5)
    {
        ui->comboBox_PinsType->removeItem(EnumToIndex(TLE5011_CS));
        ui->comboBox_PinsType->removeItem(EnumToIndex(MCP3201_CS));
        ui->comboBox_PinsType->removeItem(EnumToIndex(MCP3202_CS));
        ui->comboBox_PinsType->removeItem(EnumToIndex(MCP3204_CS));
        ui->comboBox_PinsType->removeItem(EnumToIndex(MCP3208_CS));
        ui->comboBox_PinsType->removeItem(EnumToIndex(MLX90393_CS));
        // add
        ui->comboBox_PinsType->addItem(EnumToString(SPI_MOSI));
    }
    if (pin == 6)
    {
        ui->comboBox_PinsType->removeItem(EnumToIndex(TLE5011_CS));
        // add
        ui->comboBox_PinsType->addItem(EnumToString(TLE5011_GEN));
    }
    if (pin == 8)
    {
        ui->comboBox_PinsType->addItem(EnumToString(I2C_SCL));
    }
    if (pin == 9)
    {
        ui->comboBox_PinsType->addItem(EnumToString(I2C_SDA));
    }
}

void PinComboBox::Initialization_pinC(int pin)
{
    pin_c_ = pin;
}

void PinComboBox::ReadFromConfig(uint pin)          // try?
{
    int tmp_pin = pin;  // anti-warning int and uint
    for (int i = 0; i < PIN_TYPE_COUNT; ++i) {
        if (gEnv.pDeviceConfig->config.pins[tmp_pin] == pin_list_[i].device_enum_index)
        {
            ui->comboBox_PinsType->setCurrentIndex(i);  //вместо i - ui->comboBox_PinsType->findText(pin_list_[i].gui_name)
            break;
        }
    }
}

void PinComboBox::WriteToConfig(uint pin)           // if pin = 0 try?
{
    int tmp_pin = pin;  // anti-warning int and uint
    for (int i = 0; i < PIN_TYPE_COUNT; ++i) {
        if (ui->comboBox_PinsType->currentText() == pin_list_[i].gui_name)      //GetEnumValue()
        {
            gEnv.pDeviceConfig->config.pins[tmp_pin] = pin_list_[i].device_enum_index;
        }
    }
    //            if (tmp_pin <= pin_a_){
    //                gEnv.pDeviceConfig->config.pins[tmp_pin] = pin_list_[i].device_enum_index;    // нахер я это написал?
    //            }
    //            else if (tmp_pin <= pin_b_ + pin_a_){
    //                gEnv.pDeviceConfig->config.pins[tmp_pin] = pin_list_[i].device_enum_index;
    //            }
    //            else if (tmp_pin <= pin_c_ + pin_b_ + pin_a_){          // can else without if
    //                gEnv.pDeviceConfig->config.pins[tmp_pin] = pin_list_[i].device_enum_index;
    //            }
}


int PinComboBox::GetIndex()     // ?
{
    return ui->comboBox_PinsType->currentIndex();
}
