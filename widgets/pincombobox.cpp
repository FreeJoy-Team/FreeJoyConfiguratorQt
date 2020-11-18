#include "pincombobox.h"
#include "ui_pincombobox.h"

PinComboBox::PinComboBox(QWidget *parent) :     // пины - первое, что я начал кодить в конфигураторе и спустя время
    QWidget(parent),                            // заявляю - это говнокод!1 который даже мне тяжело понять
    ui(new Ui::PinComboBox)                     // мои соболезнования тем кто будет разбираться)
{
    ui->setupUi(this);
    pin_number_ = -1;
    previous_index_ = 0;
    interact_count_ = 0;
    current_dev_enum_ = 0;

    is_call_interaction_ = false;
    is_interacts_ = false;

    styleSheet_default_ = ui->comboBox_PinsType->styleSheet();       // will not work with styles, remove from constructor, вроде пашет

    connect(ui->comboBox_PinsType, SIGNAL(currentIndexChanged(int)),
                this, SLOT(IndexChanged(int)));
}

PinComboBox::~PinComboBox()
{
    pin_types_index_.clear();         // Fault tolerant heap
    pin_types_index_.shrink_to_fit(); // Fault tolerant heap
    enum_index_.clear();
    enum_index_.shrink_to_fit();
    delete ui;
}

void PinComboBox::RetranslateUi()
{
    ui->retranslateUi(this);
}

const pins *PinComboBox::PinList() const
{
    return pin_list_;
}

//! номер элемента в pin_types
const QVector<int> & PinComboBox::PinTypeIndex() const
{
    return pin_types_index_;
}
//! device enum присутствующие в комбобоксе
const QVector<int> & PinComboBox::EnumIndex() const
{
    return enum_index_;
}

uint PinComboBox::InteractCount() const
{
    return interact_count_;
}

void PinComboBox::SetInteractCount(const uint &count)
{
    interact_count_ = count;
}

bool PinComboBox::IsInteracts() const
{
    return is_interacts_;
}

int PinComboBox::CurrentDevEnum() const
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

void PinComboBox::ResetPin()
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
        if(pin_types_[pin_types_index_[index]].device_enum_index != TLE5011_GEN)
        {
            ui->comboBox_PinsType->setEnabled(false);
        }
        is_interacts_ = true;
        ui->comboBox_PinsType->setStyleSheet(pin_types_[sender_index].styleSheet);
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

//! Set pin items
void PinComboBox::InitializationPins(uint pin)      // pin_number_ - 1 так се
{                                                   // это из-за того, что пустые значения структуры const cBox pin_types_[PIN_TYPE_COUNT]
    pin_number_ = pin;                              // инициализированы 0 и хер поймёшь код. переделаю, если будет не лень

    for (int i = 0; i < PIN_TYPE_COUNT; ++i) {
        // except
        for (int c = 0; c < 10; ++c) {
            if (pin_types_[i].pin_except[c] == 0){
                break;
            }
            if (pin_types_[i].pin_except[c] == pin_number_){
                ++i;
                break;
            }
            for (int k = 0; k < 10; ++k)
            {
                if (pin_list_[pin_number_-1].pin_type[k] == 0){
                    break;
                }
                if (pin_types_[i].pin_except[c] == pin_list_[pin_number_-1].pin_type[k]){
                    ++i;
                    break;
                }
            }
        }
        // add
        for (int j = 0; j < 10; ++j) {
            if (pin_types_[i].pin_type[j] == 0) {
                break;
            }
            if (pin_types_[i].pin_type[j] == ALL){
                ui->comboBox_PinsType->addItem(pin_types_[i].gui_name);
                pin_types_index_.push_back(i);
                enum_index_.push_back(pin_types_[i].device_enum_index);
                continue;
            }
            if (pin_types_[i].pin_type[j] == pin_number_){
                ui->comboBox_PinsType->addItem(pin_types_[i].gui_name);
                pin_types_index_.push_back(i);
                enum_index_.push_back(pin_types_[i].device_enum_index);
                continue;
            }
            for (int k = 0; k < 10; ++k)
            {
                if (pin_list_[pin_number_-1].pin_type[k] == 0){
                    break;
                }
                if (pin_types_[i].pin_type[j] == pin_list_[pin_number_-1].pin_type[k]){
                    ui->comboBox_PinsType->addItem(pin_types_[i].gui_name);
                    pin_types_index_.push_back(i);
                    enum_index_.push_back(pin_types_[i].device_enum_index);
                }
            }
        }
    }
}

void PinComboBox::IndexChanged(int index)
{

    if(!pin_types_index_.empty() && is_interacts_ == false)
    {
        ui->comboBox_PinsType->setStyleSheet(pin_types_[pin_types_index_[index]].styleSheet);      // временно?

        int tmp = 0;
        for (int i = 0; i < 10; ++i) {
            if (is_call_interaction_ == true && tmp == 0)//pin_types[pin_types_index[index]].interaction[i] <= 0
            {
                is_call_interaction_ = false;
                // add
                if (pin_types_[pin_types_index_[index]].interaction[i] > 0)
                {
                    is_call_interaction_ = true;
                    ui->comboBox_PinsType->setStyleSheet(pin_types_[pin_types_index_[index]].styleSheet);          // ?????
                    for (int t = 0; t < 10; ++t)
                    {
                        if (pin_types_[pin_types_index_[index]].interaction[t] > 0)
                        {
                            for (int k = 0; k < PIN_TYPE_COUNT; ++k) {
                                if(pin_types_[k].device_enum_index == pin_types_[pin_types_index_[index]].interaction[t]){
                                    emit valueChangedForInteraction(k, pin_types_index_[index], pin_number_);
                                    break;
                                }
                            }
                        }
                    }
                }
                // delete
                for (int n = 0; n < 10; ++n) {
                    if (pin_types_[call_interaction_].interaction[n] <= 0){
                        break;
                    }
                    if(pin_types_[call_interaction_].interaction[n] > 0){

                        for (int m = 0; m < PIN_TYPE_COUNT; ++m) {
                            if(pin_types_[m].device_enum_index == pin_types_[call_interaction_].interaction[n]){
                                emit valueChangedForInteraction(NOT_USED, m, pin_number_);
                            }
                        }
                    }
                }

                call_interaction_ = pin_types_index_[index];
                break;
            }

            // add
            else if (pin_types_[pin_types_index_[index]].interaction[i] > 0)
            {
                is_call_interaction_ = true;
                ui->comboBox_PinsType->setStyleSheet(pin_types_[pin_types_index_[index]].styleSheet);        // ????
                for (int k = 0; k < PIN_TYPE_COUNT; ++k) {
                    if(pin_types_[k].device_enum_index == pin_types_[pin_types_index_[index]].interaction[i]){
                        call_interaction_ = pin_types_index_[index];
                        emit valueChangedForInteraction(k, pin_types_index_[index], pin_number_);
                        tmp++;
                        break;
                    }
                }
            }
        }
    }

    // set current config
    if(!pin_types_index_.empty()){
        emit currentIndexChanged(enum_index_[index], previous_index_, pin_number_);
        previous_index_ = enum_index_[index];
        current_dev_enum_ = enum_index_[index];
    }

}


//void PinComboBox::resizeEvent(QResizeEvent*)
//{
////    static bool tmp = false;
////    if(tmp == false)
////    {
//        index2 = pin_types_index[ui->comboBox_PinsType->currentIndex()];
//        name2 = pin_types[index2].gui_name;
//        width2 = ui->comboBox_PinsType->width()/2 - name2.length()*3;
////        tmp = true;
////    }

//    QString padding = QString::number(width2);
//    QString styleSheet = "QComboBox { padding-left: %1px;}";
//    ui->comboBox_PinsType->setStyleSheet( styleSheet.arg(padding) );
//    //ui->comboBox_PinsType->setStyleSheet(QLatin1String("QComboBox QListView {padding-left: 20px;}"));
//}


void PinComboBox::ReadFromConfig(uint pin)          // try?
{
    for (int i = 0; i < enum_index_.size(); ++i) {
        if (gEnv.pDeviceConfig->config.pins[pin] == enum_index_[i])
        {
            ui->comboBox_PinsType->setCurrentIndex(int(i));
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
