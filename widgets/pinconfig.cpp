#include "pinconfig.h"
#include "ui_pinconfig.h"

//#include <QtConcurrent/QtConcurrent>
//#include <QFuture>

#define PINS_IN_GROUP_RANGE 16          // range A0-A15 = 16
#include <QDebug>
PinConfig::PinConfig(QWidget *parent) :         // пины - первое, что я начал кодить в конфигураторе и спустя время
    QWidget(parent),                            // заявляю - это говнокод!1 который даже мне тяжело понять
    ui(new Ui::PinConfig)                       // мои соболезнования тем кто будет разбираться)
{
    ui->setupUi(this);
    max_buttons_warning_ = false;
    pin_count_ = 0;
    shift_latch_count_ = 0;
    shift_data_count_ = 0;

    axis_sources_ = 0;
    buttons_from_axes_ = 0;
    buttons_from_shift_regs_ = 0;
    single_buttons_ = 0;
    rows_of_buttons_ = 0;
    columns_of_buttons_ = 0;
    single_LED_ = 0;
    rows_of_LED_ = 0;
    columns_of_LED_ = 0;

//    FilterObject *filter = new FilterObject();  // delete
//    filter->target=ui->label_ControllerImage;
//    ui->label_ControllerImage->installEventFilter(filter);

//    QPixmap pix(":/Images/BluePill_pic2.jpg"); // load pixmap
//    // get label dimensions
//    int w = ui->label_ControllerImage->width();
//    int h = ui->label_ControllerImage->height();
//    // set a scaled pixmap to a w x h window keeping its aspect ratio
//    ui->label_ControllerImage->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));

    int number = 1;
    QString name_template_pinA(QStringLiteral("widgetPin_PA%1"));//QtConcurrent::run()
    for(uint8_t i = 0; i < 16; i++) {                   // 16 to PINS_IN_GROUP_RANGE
        PinComboBox *pinComboBox = this->findChild<PinComboBox *>(name_template_pinA.arg(i));
        if (pinComboBox != nullptr){
            pin_count_++;
            //QFuture<void> future = QtConcurrent::run(pinComboBox, &PinComboBox::InitializationPins, number++);      // hz
            pinComboBox->InitializationPins(number++);
            PinCBoxPtrList_.append(pinComboBox);
        }
    }
    // search and initializate pins PB
    QString name_template_pinB(QStringLiteral("widgetPin_PB%1"));
    for(uint8_t i = 0; i < 16; i++) {
        PinComboBox *pinComboBox = this->findChild<PinComboBox *>(name_template_pinB.arg(i));
        if (pinComboBox != nullptr){
            pin_count_++;
            //QFuture<void> future = QtConcurrent::run(pinComboBox, &PinComboBox::InitializationPins, number++);      // hz
            pinComboBox->InitializationPins(number++);
            PinCBoxPtrList_.append(pinComboBox);
        }
    }
    // search and initializate pins PC
    QString name_template_pinC(QStringLiteral("widgetPin_PC%1"));
    for(uint8_t i = 0; i < 16; i++) {
        PinComboBox *pinComboBox = this->findChild<PinComboBox *>(name_template_pinC.arg(i));
        if (pinComboBox != nullptr){
            pin_count_++;
            //QFuture<void> future = QtConcurrent::run(pinComboBox, &PinComboBox::InitializationPins, number++);      // hz
            pinComboBox->InitializationPins(number++);
            PinCBoxPtrList_.append(pinComboBox);
        }
    }

    for (int i = 0; i < PinCBoxPtrList_.size(); ++i) {
            connect(PinCBoxPtrList_[i], SIGNAL(valueChangedForInteraction(int, int, int)),       // valgrind сообщает о утечке, но почему?
                        this, SLOT(pinInteraction(int, int, int)));
            connect(PinCBoxPtrList_[i], SIGNAL(currentIndexChanged(int, int, int)),
                        this, SLOT(pinIndexChanged(int, int, int)));
    }
    connect(this, SIGNAL(totalButtonsValueChanged(int)),
            this, SLOT(totalButtonsChanged(int)));

    if (pin_count_ != PINS_COUNT){
        qCritical()<<"pinconfig.cpp pin_count_ != PINS_COUNT";
    }
}

PinConfig::~PinConfig()
{
    delete ui;
}

void PinConfig::RetranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < PinCBoxPtrList_.size(); ++i) {
        PinCBoxPtrList_[i]->RetranslateUi();
    }
}

void PinConfig::pinInteraction(int index, int sender_index, int pin)
{
    if (index != NOT_USED)//current_enum_index
    {
        for (int i = 0; i < PinCBoxPtrList_.size(); ++i) {
            for (int j = 0; j < PinCBoxPtrList_[i]->PinTypeIndex().size(); ++j) {
                if (PinCBoxPtrList_[i]->PinTypeIndex()[j] == index)
                {
                    if(PinCBoxPtrList_[i]->InteractCount() == 0){
                        // как-то некрасиво
                        PinCBoxPtrList_[i]->SetInteractCount(PinCBoxPtrList_[i]->InteractCount() + pin);
                        PinCBoxPtrList_[i]->SetIndex_Iteraction(j, sender_index);
                    }
                    else if (PinCBoxPtrList_[i]->IsInteracts() == true){
                        PinCBoxPtrList_[i]->SetInteractCount(PinCBoxPtrList_[i]->InteractCount() + pin);
                    }
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < PinCBoxPtrList_.size(); ++i) {
            if (PinCBoxPtrList_[i]->IsInteracts() == true)
            {
                for (int j = 0; j < PinCBoxPtrList_[i]->PinTypeIndex().size(); ++j) {
                    if (PinCBoxPtrList_[i]->PinTypeIndex()[j] == sender_index)
                    {
                        if(PinCBoxPtrList_[i]->InteractCount() > 0){
                            PinCBoxPtrList_[i]->SetInteractCount(PinCBoxPtrList_[i]->InteractCount() - pin);
                        }
                        if (PinCBoxPtrList_[i]->InteractCount() <= 0) {
                            PinCBoxPtrList_[i]->SetIndex_Iteraction(0, sender_index);
                        }
                    }
                }
            }
        }
    }
}


void PinConfig::pinIndexChanged(int current_device_enum, int previous_device_enum, int pin_number)              // мб сделать сразу запись в конфиг из пинов
{                                                                                                               // или отдельный класс для их состояний
    // signals for another widgets
    SignalsForWidgets(current_device_enum, previous_device_enum, pin_number);

    // pin type limit           // переизбыток функционала(изи менять в структуре), не думаю, что понадобится в будущем, можно было и захардкодить
    PinTypeLimit(current_device_enum, previous_device_enum);

    // set current config and generate signals for another widgets
//    else {
    SetCurrentConfig(current_device_enum, previous_device_enum, pin_number);
    //}
}


void PinConfig::SignalsForWidgets(int current_device_enum, int previous_device_enum, int pin_number)
{
    //fast encoder selected
    if (current_device_enum == FAST_ENCODER){
        emit fastEncoderSelected(PinCBoxPtrList_[0]->PinList()[pin_number - PA_0].gui_name, true);    // hz
    } else if (previous_device_enum == FAST_ENCODER){
        emit fastEncoderSelected(PinCBoxPtrList_[0]->PinList()[pin_number - PA_0].gui_name, false);    // hz
    }
    // shift register latch selected
    if (current_device_enum == SHIFT_REG_LATCH){
        shift_latch_count_++;
        emit shiftRegSelected(pin_number, 0, PinCBoxPtrList_[0]->PinList()[pin_number - PA_0].gui_name);    // hz
    } else if (previous_device_enum == SHIFT_REG_LATCH){
        shift_latch_count_--;
        emit shiftRegSelected((pin_number)*-1, 0, PinCBoxPtrList_[0]->PinList()[pin_number - PA_0].gui_name);    // hz
    }
    // shift register data selected
    if (current_device_enum == SHIFT_REG_DATA){
        shift_data_count_++;
        emit shiftRegSelected(0, pin_number, PinCBoxPtrList_[0]->PinList()[pin_number - PA_0].gui_name);    // hz
    } else if (previous_device_enum == SHIFT_REG_DATA){
        shift_data_count_--;
        emit shiftRegSelected(0, (pin_number)*-1, PinCBoxPtrList_[0]->PinList()[pin_number - PA_0].gui_name);    // hz
    }
    // I2C selected
    if (current_device_enum == I2C_SCL){// || current_device_enum == I2C_SDA){
        emit axesSourceChanged(-2, true);                                            // -2 enum I2C в axes.h
    } else if (previous_device_enum == I2C_SCL){// || previous_device_enum == I2C_SDA){
        emit axesSourceChanged(-2, false);
    }
}

void PinConfig::PinTypeLimit(int current_device_enum, int previous_device_enum)
{
    static int limit_count_array[PIN_TYPE_LIMIT_COUNT]{};
    static bool limit_is_enable[PIN_TYPE_LIMIT_COUNT]{};

    for (int i = 0; i < PIN_TYPE_LIMIT_COUNT; ++i)
    {
        if (current_device_enum == pin_type_limit_[i].device_enum_index)
        {
            limit_count_array[i]++;
        }
        if (previous_device_enum == pin_type_limit_[i].device_enum_index)
        {
            limit_count_array[i]--;
        }

        if (limit_count_array[i] >= pin_type_limit_[i].max_count && limit_is_enable[i] == false)
        {
            limit_is_enable[i] = true;
            for (int j = 0; j < PinCBoxPtrList_.size(); ++j)
            {
                for (int k = 0; k < PinCBoxPtrList_[j]->EnumIndex().size(); ++k) {
                    if (PinCBoxPtrList_[j]->EnumIndex()[k] == pin_type_limit_[i].device_enum_index &&
                        PinCBoxPtrList_[j]->CurrentDevEnum() != current_device_enum)
                    {
                        PinCBoxPtrList_[j]->SetIndexStatus(int(k), false);
                    }
                }
            }
        }

        if (limit_is_enable[i] == true && limit_count_array[i] < pin_type_limit_[i].max_count)
        {
            limit_is_enable[i] = false;
            for (int j = 0; j < PinCBoxPtrList_.size(); ++j)
            {
                for (int k = 0; k < PinCBoxPtrList_[j]->EnumIndex().size(); ++k) {
                    if (PinCBoxPtrList_[j]->EnumIndex()[k] == pin_type_limit_[i].device_enum_index)
                    {
                        PinCBoxPtrList_[j]->SetIndexStatus(int(k), true);
                    }
                }
            }
        }
    }
}

void PinConfig::SetCurrentConfig(int current_device_enum, int previous_device_enum, int pin_number)
{
    for (int i = 0; i < SOURCE_COUNT; ++i) {
        for (int j = 0; j < PIN_TYPE_COUNT; ++j) {
            if(source[i].pin_type[j] == 0){
                break;
            }
            else if(source[i].pin_type[j] == current_device_enum || source[i].pin_type[j] == previous_device_enum){

                int tmp;
                if (source[i].pin_type[j] == current_device_enum){
                    tmp = 1;
                } else {
                    tmp = -1;
                }

                if (i == AXIS_SOURCE){      //int source_enum, bool is_add      axesSourceChanged
                    axis_sources_+=tmp;
                    ui->label_AxisSources->setNum(axis_sources_);
                    if (tmp > 0){
                        emit axesSourceChanged(pin_number - 1, true);
                    } else {
                        emit axesSourceChanged(pin_number - 1, false);
                    }
                }
                else if (i == SINGLE_BUTTON){
                    single_buttons_+=tmp;
                    ui->label_SingleButtons->setNum(single_buttons_);
                    ui->label_TotalButtons->setNum(buttons_from_shift_regs_ + buttons_from_axes_ + single_buttons_ + (columns_of_buttons_ * rows_of_buttons_));
                    emit totalButtonsValueChanged(buttons_from_shift_regs_ + buttons_from_axes_ + single_buttons_ + (columns_of_buttons_ * rows_of_buttons_));
                }
                else if (i == ROW_OF_BUTTONS){
                    rows_of_buttons_+=tmp;
                    ui->label_RowsOfButtons->setNum(rows_of_buttons_);
                    ui->label_TotalButtons->setNum(buttons_from_shift_regs_ + buttons_from_axes_ + single_buttons_ + (columns_of_buttons_ * rows_of_buttons_));
                    emit totalButtonsValueChanged(buttons_from_shift_regs_ + buttons_from_axes_ + single_buttons_ + (columns_of_buttons_ * rows_of_buttons_));
                }
                else if (i == COLUMN_OF_BUTTONS){
                    columns_of_buttons_+=tmp;
                    ui->label_ColumnsOfButtons->setNum(columns_of_buttons_);
                    ui->label_TotalButtons->setNum(buttons_from_shift_regs_ + buttons_from_axes_ + single_buttons_ + (columns_of_buttons_ * rows_of_buttons_));
                    emit totalButtonsValueChanged(buttons_from_shift_regs_ + buttons_from_axes_ + single_buttons_ + (columns_of_buttons_ * rows_of_buttons_));
                }
                else if (i == SINGLE_LED){
                    single_LED_+=tmp;
                    ui->label_TotalLEDs->setNum(single_LED_ + (rows_of_LED_ * columns_of_LED_));
                    emit totalLEDsValueChanged(single_LED_ + (rows_of_LED_ * columns_of_LED_));
                }
                else if (i == ROW_OF_LED){
                    rows_of_LED_+=tmp;
                    ui->label_TotalLEDs->setNum(single_LED_ + (rows_of_LED_ * columns_of_LED_));
                    emit totalLEDsValueChanged(single_LED_ + (rows_of_LED_ * columns_of_LED_));
                }
                else if (i == COLUMN_OF_LED){
                    columns_of_LED_+=tmp;
                    ui->label_TotalLEDs->setNum(single_LED_ + (rows_of_LED_ * columns_of_LED_));
                    emit totalLEDsValueChanged(single_LED_ + (rows_of_LED_ * columns_of_LED_));
                }
            }
        }
    }
}



void PinConfig::ResetAllPins()
{
    for (int i = 0; i < PinCBoxPtrList_.size(); ++i) {
        PinCBoxPtrList_[i]->ResetPin();
    }
}

void PinConfig::a2bCountChanged(int count)
{
    buttons_from_axes_ = count;
    ui->label_ButtonFromAxes->setNum(buttons_from_axes_);
    ui->label_TotalButtons->setNum(buttons_from_shift_regs_ + buttons_from_axes_ + single_buttons_ + (columns_of_buttons_ * rows_of_buttons_));
    emit totalButtonsValueChanged(buttons_from_shift_regs_ + buttons_from_axes_ + single_buttons_ + (columns_of_buttons_ * rows_of_buttons_));
}

void PinConfig::shiftRegButtonsCountChanged(int count)
{
    buttons_from_shift_regs_ = count;
    ui->label_ButtonsFromShiftRegs->setNum(buttons_from_shift_regs_);
    ui->label_TotalButtons->setNum(buttons_from_shift_regs_ + buttons_from_axes_ + single_buttons_ + (columns_of_buttons_ * rows_of_buttons_));
    emit totalButtonsValueChanged(buttons_from_shift_regs_ + buttons_from_axes_ + single_buttons_ + (columns_of_buttons_ * rows_of_buttons_));
}

void PinConfig::totalButtonsChanged(int count)
{
    if (count > MAX_BUTTONS_NUM){
        default_label_style_ = ui->label_TotalButtons->styleSheet();
        //ui->label_TotalButtons->setStyleSheet(default_label_style_ + "background-color: rgb(200, 0, 0);");
        ui->text_TotalButtons->setStyleSheet(default_label_style_ + QStringLiteral("background-color: rgb(200, 0, 0);"));
        max_buttons_warning_ = true;
    } else if (max_buttons_warning_ == true){   // && count <= MAX_BUTTONS_NUM
        //ui->label_TotalButtons->setStyleSheet(default_label_style_);
        ui->text_TotalButtons->setStyleSheet(default_label_style_);
        max_buttons_warning_ = false;
    }
}


//void PinConfig::resizeEvent(QResizeEvent* event)
//{
//    QPixmap pix(":/Images/BluePill_pic2.jpg"); // load pixmap
//    // get label dimensions
//    int w = ui->label_ControllerImage->width();
//    int h = ui->label_ControllerImage->height();
//    // set a scaled pixmap to a w x h window keeping its aspect ratio
//    ui->label_ControllerImage->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatioByExpanding));
//    QSize size = pix.size();
//    ui->label_ControllerImage->setMaximumSize(size.width(), size.height());
//}


void PinConfig::ReadFromConfig(){
    for (uint i = 0; i < pin_count_; ++i) {
        PinCBoxPtrList_[i]->ReadFromConfig(i);
    }
}

void PinConfig::WriteToConfig(){
    for (uint i = 0; i < pin_count_; ++i) {
        PinCBoxPtrList_[i]->WriteToConfig(i);
    }
}

