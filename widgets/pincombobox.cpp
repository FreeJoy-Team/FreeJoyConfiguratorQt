#include "pincombobox.h"
#include "ui_pincombobox.h"


PinComboBox::PinComboBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PinComboBox)
{
    ui->setupUi(this);
    pin_number_ = -1;
    previous_index_ = 0;
    interact_count_ = 0;
    current_dev_enum_ = 0;

    is_call_interaction_ = false;
    is_interacts_ = false;

    last_deleted_ = 0;  // ???

    styleSheet_default_ = ui->comboBox_PinsType->styleSheet();       // will not work with styles, remove from constructor

    connect(ui->comboBox_PinsType, SIGNAL(currentIndexChanged(int)),
                this, SLOT(IndexChanged(int)));

}

PinComboBox::~PinComboBox()
{
    pin_types_index.clear();         // Fault tolerant heap
    pin_types_index.shrink_to_fit(); // Fault tolerant heap
    enum_index.clear();
    enum_index.shrink_to_fit();
    delete ui;
}

void PinComboBox::RetranslateUi()
{
    ui->retranslateUi(this);
}

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
                pin_types_index.push_back(i);
                enum_index.push_back(pin_types[i].device_enum_index);
                continue;
            }
            if (pin_types[i].pin_type[j] == pin_number_){
                ui->comboBox_PinsType->addItem(pin_types[i].gui_name);
                pin_types_index.push_back(i);
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
                    pin_types_index.push_back(i);
                    enum_index.push_back(pin_types[i].device_enum_index);
                }
            }
        }
    }
}

void PinComboBox::IndexChanged(int index)
{
    // pins interaction
    if(pin_types_index.size() && is_interacts_ == false)
    {
        ui->comboBox_PinsType->setStyleSheet(pin_types[pin_types_index[index]].styleSheet);      // временно?

        int tmp = 0;
        for (size_t i = 0; i < 10; ++i) {
            if (is_call_interaction_ == true && tmp == 0)//pin_types[pin_types_index[index]].interaction[i] <= 0
            {
                is_call_interaction_ = false;
                //ui->comboBox_PinsType->setStyleSheet(styleSheet_default_);          // ?????
                // add
                if (pin_types[pin_types_index[index]].interaction[i] > 0)
                {
                    //qDebug()<<"add";
                    is_call_interaction_ = true;
                    ui->comboBox_PinsType->setStyleSheet(pin_types[pin_types_index[index]].styleSheet);          // ?????
                    for (int t = 0; t < 10; ++t)
                    {
                      // if (is_call_interaction_ == true){

                            if (pin_types[pin_types_index[index]].interaction[t] > 0)
                            {
                                //qDebug()<<"add";
                                //is_call_interaction_ = true;
                                //ui->comboBox_PinsType->setStyleSheet(pin_types[pin_types_index[index]].styleSheet);          // ?????n
                                for (int k = 0; k < PIN_TYPE_COUNT; ++k) {
                                    if(pin_types[k].device_enum_index == pin_types[pin_types_index[index]].interaction[t]){
                                        emit valueChangedForInteraction(k, pin_types_index[index], pin_number_);
                                        //qDebug()<<"add";
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
                                emit valueChangedForInteraction(NOT_USED, m, pin_number_);
                            }
                        }
                    }
                }

                call_interaction_ = pin_types_index[index];
                break;
            }

            // add
            else if (pin_types[pin_types_index[index]].interaction[i] > 0)
            {
                is_call_interaction_ = true;
                ui->comboBox_PinsType->setStyleSheet(pin_types[pin_types_index[index]].styleSheet);        // ????
                for (int k = 0; k < PIN_TYPE_COUNT; ++k) {
                    if(pin_types[k].device_enum_index == pin_types[pin_types_index[index]].interaction[i]){
                        //is_interacts_++;
                        call_interaction_ = pin_types_index[index];
                        emit valueChangedForInteraction(k, pin_types_index[index], pin_number_);
                        tmp++;
                        break;
                    }
                }
            }
        }
    }

    // set current config
    //if(enum_index[index] == )
    if(pin_types_index.size()){
        emit currentIndexChanged(enum_index[index], previous_index_, pin_number_);
        previous_index_ = enum_index[index];
        current_dev_enum_ = enum_index[index];
    }
}


//int PinComboBox::GetEnumValue()     //????????   // old, delete
//{
//    for (int i = 0; i < PIN_TYPE_COUNT; ++i) {
//        if (ui->comboBox_PinsType->currentText() == pin_types[i].gui_name)
//        {
//            return pin_types[i].device_enum_index;
//        }
//    }
//    return 0;
//    //emit valueChanged2(pin_list_index);
//}

////! Returns a string to add to ComboBox         // old, delete
//QString PinComboBox::EnumToString(int deviceEnum)               // try catch
//{
//    for (int i = 0; i < PIN_TYPE_COUNT; i++){
//        if (pin_types[i].device_enum_index == deviceEnum){
//            return pin_types[i].gui_name;
//        }
//    }
//    return "Not Found";
//}

////! Returns the index to remove from ComboBox   // old, delete
//int PinComboBox::EnumToIndex(int deviceEnum)                    // try catch
//{
//    QString string;
//    for (int i = 0; i < PIN_TYPE_COUNT; i++){
//        if (pin_types[i].device_enum_index == deviceEnum){
//            string = pin_types[i].gui_name;
//            break;
//        }
//    }
//    return ui->comboBox_PinsType->findText(string);
//}


int PinComboBox::GetIndex()     // ?
{
    return ui->comboBox_PinsType->currentIndex();
}

int PinComboBox::GetCurrentDevEnum()
{
    return current_dev_enum_;
}
//! Set selected index enable or disable
void PinComboBox::SetIndexStatus(int index, bool status)
{
    if (status == true){
        ui->comboBox_PinsType->setItemData(index, 1 | 32, Qt::UserRole - 1);
    } else {
        ui->comboBox_PinsType->setItemData(index, 0, Qt::UserRole - 1);
    }
}

void PinComboBox::ResetPin()     // ?
{
    ui->comboBox_PinsType->setCurrentIndex(0);
    if (is_interacts_ == true)
    {
        ui->comboBox_PinsType->setEnabled(true);
        is_interacts_ = false;
        ui->comboBox_PinsType->setStyleSheet(styleSheet_default_);
    }
}

void PinComboBox::SetIndex_Iteraction(int index, int sender_index)
{
    if(is_interacts_ == false && is_call_interaction_ == false)     // ui->comboBox_PinsType->isEnabled()
    {
        if(pin_types[pin_types_index[index]].device_enum_index != TLE5011_GEN)
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
    //int tmp_pin = pin;  // anti-warning int and uint      // size_t  pin_types_index[i]
    for (size_t i = 0; i < enum_index.size(); ++i) {
        if (gEnv.pDeviceConfig->config.pins[pin] == enum_index[i])
        {
            //qDebug()<<i;
            ui->comboBox_PinsType->setCurrentIndex(int(i));  //вместо i - ui->comboBox_PinsType->findText(pin_list_[i].gui_name)
            break;
        }
    }
}

void PinComboBox::WriteToConfig(uint pin)           // if pin = 0 try?
{   
    gEnv.pDeviceConfig->config.pins[pin] = current_dev_enum_;


//    // МЕДЛЕННО, НАХЕРА Я СДЕЛАЛ ПОИСК ПО ТЕКСТУ?!    пиздец
//    for (size_t i = 0; i < PIN_TYPE_COUNT; ++i) {
//        if (ui->comboBox_PinsType->currentText() == pin_types[i].gui_name)      //GetEnumValue()
//        {
//            gEnv.pDeviceConfig->config.pins[pin] = pin_types[i].device_enum_index;
//        }
//    }
}
