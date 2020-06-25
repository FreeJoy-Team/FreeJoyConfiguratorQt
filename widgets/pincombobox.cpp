#include "pincombobox.h"
#include "ui_pincombobox.h"

#define PIN_TYPES_FOR_ALL 16

PinComboBox::PinComboBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PinComboBox)
{
    ui->setupUi(this);
    // во все комбобоксы из pin_list_ в количестве PIN_TYPES_FOR_ALL
//    for (int i = 0; i < PIN_TYPES_FOR_ALL; i++) {
//        ui->comboBox_PinsType->addItem(pin_list_[i].gui_name);
//    }
    styleSheet_default_ = ui->comboBox_PinsType->styleSheet();                  // will not work with styles, remove from constructor

    connect(ui->comboBox_PinsType, SIGNAL(currentIndexChanged(int)),
                this, SLOT(IndexChanged(int)));

}

struct cBox22
{
    int device_enum_index;      // контроллер enum
    QString gui_name;           // имя в gui
    int pin_type[10];           // добавление в конкретный пин или в пины типа
    int pin_except[10];         // исключая пины или пины типа
    int interaction[10];        // взаимодействие с другими типами
    QString styleSheet;         // стиль взаимодействия
};
struct PIIIn
{

};

//! Set pin items
void PinComboBox::InitializationPins(uint pin)      // pin_number_ - 1 так се
{
    pin_number_ = pin;

    for (int i = 0; i < PIN_TYPE_COUNT; ++i) {
        // except
        for (int c = 0; c < 10; ++c) {
            if (pin_types[i].pin_except[c] == 0){
                break;
            }
            if (pin_types[i].pin_except[c] == pin_number_){
                ++i;
                break;
            }
            for (int k = 0; k < 10; ++k)
            {
                if (pin_list[pin_number_-1].pin_type[k] == 0){
                    break;
                }
                if (pin_types[i].pin_except[c] == pin_list[pin_number_-1].pin_type[k]){
                    ++i;
                    break;
                }
            }
        }
        // add
        for (int j = 0; j < 10; ++j) {
            if (pin_types[i].pin_type[j] == 0) {
                break;
            }
            if (pin_types[i].pin_type[j] == ALL){
                ui->comboBox_PinsType->addItem(pin_types[i].gui_name);
                enum_gui_index.push_back(i);
                enum_index.push_back(pin_types[i].device_enum_index);
                continue;
            }
            if (pin_types[i].pin_type[j] == pin_number_){
                ui->comboBox_PinsType->addItem(pin_types[i].gui_name);
                enum_gui_index.push_back(i);
                enum_index.push_back(pin_types[i].device_enum_index);
                continue;
            }
            for (int k = 0; k < 10; ++k)
            {
                if (pin_list[pin_number_-1].pin_type[k] == 0){
                    break;
                }
                if (pin_types[i].pin_type[j] == pin_list[pin_number_-1].pin_type[k]){
                    ui->comboBox_PinsType->addItem(pin_types[i].gui_name);
                    enum_gui_index.push_back(i);
                    enum_index.push_back(pin_types[i].device_enum_index);
                }
            }
        }
    }
    //qDebug()<<enum_gui_index.size();
    //qDebug()<<enum_gui_index[pin];
    for (size_t i = 0; i < enum_gui_index.size(); ++i) {
        qDebug()<<enum_gui_index[i];
    }
    qDebug()<< "Done"<<pin;
}


PinComboBox::~PinComboBox()
{
    enum_gui_index.clear();         // Fault tolerant heap
    enum_gui_index.shrink_to_fit(); // Fault tolerant heap
    enum_index.clear();
    enum_index.shrink_to_fit();
    delete ui;
}

void PinComboBox::IndexChanged(int index)
{
    if(enum_gui_index.size() && is_interacts_ == false)
    {
        ui->comboBox_PinsType->setStyleSheet(pin_types[enum_gui_index[index]].styleSheet);      // временно

        int tmp = 0;
        for (size_t i = 0; i < 10; ++i) {
            if (is_call_interaction_ == true && tmp == 0)//pin_types[enum_gui_index[index]].interaction[i] <= 0
            {
                is_call_interaction_ = false;
                ui->comboBox_PinsType->setStyleSheet(styleSheet_default_);          // ?????
                // add
                if (pin_types[enum_gui_index[index]].interaction[i] > 0)
                {
                    //qDebug()<<"add";
                    is_call_interaction_ = true;
                    ui->comboBox_PinsType->setStyleSheet(pin_types[enum_gui_index[index]].styleSheet);          // ?????
                    for (int t = 0; t < 10; ++t)
                    {
                      // if (is_call_interaction_ == true){

                            if (pin_types[enum_gui_index[index]].interaction[t] > 0)
                            {
                                //qDebug()<<"add";
                                //is_call_interaction_ = true;
                                //ui->comboBox_PinsType->setStyleSheet(pin_types[enum_gui_index[index]].styleSheet);          // ?????
                                for (int k = 0; k < PIN_TYPE_COUNT; ++k) {
                                    if(pin_types[k].device_enum_index == pin_types[enum_gui_index[index]].interaction[t]){
                                        emit valueChanged(k, enum_gui_index[index], pin_number_);
                                        qDebug()<<"add";
                                        break;
                                    }
                                }
                            }
                        //}
                    }
                }
                // delete
                for (int n = 0; n < 10; ++n) {
                    if (pin_types[call_interaction_].interaction[n] <= 0){
                        break;
                    }
                    if(pin_types[call_interaction_].interaction[n] > 0){

                        for (int m = 0; m < PIN_TYPE_COUNT; ++m) {
                            if(pin_types[m].device_enum_index == pin_types[call_interaction_].interaction[n]){
                                emit valueChanged(NOT_USED, m, pin_number_);
                            }
                        }
                    }
                }

                call_interaction_ = enum_gui_index[index];
                break;
            }

            // add
            else if (pin_types[enum_gui_index[index]].interaction[i] > 0)
            {
                is_call_interaction_ = true;
                ui->comboBox_PinsType->setStyleSheet(pin_types[enum_gui_index[index]].styleSheet);
                for (int k = 0; k < PIN_TYPE_COUNT; ++k) {
                    if(pin_types[k].device_enum_index == pin_types[enum_gui_index[index]].interaction[i]){
                        //is_interacts_++;
                        call_interaction_ = enum_gui_index[index];
                        emit valueChanged(k, enum_gui_index[index], pin_number_);
                        tmp++;
                        break;
                    }
                }
            }
        }
    }
}


int PinComboBox::GetEnumValue()     //????????
{
    for (int i = 0; i < PIN_TYPE_COUNT; ++i) {
        if (ui->comboBox_PinsType->currentText() == pin_types[i].gui_name)
        {
            return pin_types[i].device_enum_index;
        }
    }
    return 0;
    //emit valueChanged2(pin_list_index);
}

//! Returns a string to add to ComboBox
QString PinComboBox::EnumToString(int deviceEnum)               // try catch
{
    for (int i = 0; i < PIN_TYPE_COUNT; i++){
        if (pin_types[i].device_enum_index == deviceEnum){
            return pin_types[i].gui_name;
        }
    }
    return "Not Found";
}

//! Returns the index to remove from ComboBox
int PinComboBox::EnumToIndex(int deviceEnum)                    // try catch
{
    QString string;
    for (int i = 0; i < PIN_TYPE_COUNT; i++){
        if (pin_types[i].device_enum_index == deviceEnum){
            string = pin_types[i].gui_name;
            break;
        }
    }
    return ui->comboBox_PinsType->findText(string);
}


int PinComboBox::GetIndex()     // ?
{
    return ui->comboBox_PinsType->currentIndex();
}

void PinComboBox::SetIndex(int index, int sender_index)
{
    if(is_interacts_ == false && is_call_interaction_ == false)     // ui->comboBox_PinsType->isEnabled()
    {
        if(pin_types[enum_gui_index[index]].device_enum_index != TLE5011_GEN)
        {
            ui->comboBox_PinsType->setEnabled(false);
        }
        is_interacts_ = true;
        ui->comboBox_PinsType->setStyleSheet(pin_types[sender_index].styleSheet);
        ui->comboBox_PinsType->setCurrentIndex(index);
    }
    else if (is_interacts_ == true)// && interact_count_ <= 0)
    {
        ui->comboBox_PinsType->setEnabled(true);
        is_interacts_ = false;
        ui->comboBox_PinsType->setStyleSheet(styleSheet_default_);
        ui->comboBox_PinsType->setCurrentIndex(index);
    }
}







void PinComboBox::ReadFromConfig(uint pin)          // try?
{
    //int tmp_pin = pin;  // anti-warning int and uint      // size_t  enum_gui_index[i]
    for (size_t i = 0; i < enum_index.size(); ++i) {
        if (gEnv.pDeviceConfig->config.pins[pin] == enum_index[i])
        {
            qDebug()<<i;
            ui->comboBox_PinsType->setCurrentIndex(i);  //вместо i - ui->comboBox_PinsType->findText(pin_list_[i].gui_name)
            break;
        }
    }
}

void PinComboBox::WriteToConfig(uint pin)           // if pin = 0 try?      // size_t
{
    //int tmp_pin = pin;  // anti-warning int and uint
    for (size_t i = 0; i < PIN_TYPE_COUNT; ++i) {
        if (ui->comboBox_PinsType->currentText() == pin_types[i].gui_name)      //GetEnumValue()
        {
            gEnv.pDeviceConfig->config.pins[pin] = pin_types[i].device_enum_index;
        }
    }
}
