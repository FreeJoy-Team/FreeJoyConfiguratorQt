#include "pinconfig.h"
#include "ui_pinconfig.h"

//#include "mousewheelguard.h"

//#include <QtConcurrent/QtConcurrent>
//#include <QFuture>

#define PINS_IN_GROUP_RANGE 16          // range A0-A15 = 16

PinConfig::PinConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PinConfig)
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
    QString name_template_pinA("widgetPin_PA%1");//QtConcurrent::run()
    for(uint8_t i = 0; i < 16; i++) {                   // 16 to PINS_IN_GROUP_RANGE
        PinComboBox *pinComboBox = this->findChild<PinComboBox *>(name_template_pinA.arg(i));
        if (pinComboBox != nullptr){
            pin_count_++;
            //QFuture<void> future = QtConcurrent::run(pinComboBox, &PinComboBox::InitializationPins, number++);      // hz
            pinComboBox->InitializationPins(number++);
            PinComboBoxPtrList.append(pinComboBox);
        }
    }
    // search and initializate pins PB
    QString name_template_pinB("widgetPin_PB%1");
    for(uint8_t i = 0; i < 16; i++) {
        PinComboBox *pinComboBox = this->findChild<PinComboBox *>(name_template_pinB.arg(i));
        if (pinComboBox != nullptr){
            pin_count_++;
            //QFuture<void> future = QtConcurrent::run(pinComboBox, &PinComboBox::InitializationPins, number++);      // hz
            pinComboBox->InitializationPins(number++);
            PinComboBoxPtrList.append(pinComboBox);
        }
    }
    // search and initializate pins PC
    QString name_template_pinC("widgetPin_PC%1");
    for(uint8_t i = 0; i < 16; i++) {
        PinComboBox *pinComboBox = this->findChild<PinComboBox *>(name_template_pinC.arg(i));
        if (pinComboBox != nullptr){
            pin_count_++;
            //QFuture<void> future = QtConcurrent::run(pinComboBox, &PinComboBox::InitializationPins, number++);      // hz
            pinComboBox->InitializationPins(number++);
            PinComboBoxPtrList.append(pinComboBox);
        }
    }

    for (int i = 0; i < PinComboBoxPtrList.size(); ++i) {
            connect(PinComboBoxPtrList[i], SIGNAL(valueChangedForInteraction(int, int, int)),       // valgrind сообщает о утечке, но почему?
                        this, SLOT(pinInteraction(int, int, int)));
            connect(PinComboBoxPtrList[i], SIGNAL(currentIndexChanged(int, int, int)),
                        this, SLOT(pinIndexChanged(int, int, int)));
    }
    connect(this, SIGNAL(totalButtonsValueChanged(int)),
            this, SLOT(totalButtonsChanged(int)));
}

PinConfig::~PinConfig()
{
    delete ui;
}

void PinConfig::RetranslateUi()
{
    ui->retranslateUi(this);
    for (int i = 0; i < PinComboBoxPtrList.size(); ++i) {
        PinComboBoxPtrList[i]->RetranslateUi();
    }
}

void PinConfig::pinInteraction(int index, int sender_index, int pin)
{
    if (index != NOT_USED)//current_enum_index
    {
        for (int i = 0; i < PinComboBoxPtrList.size(); ++i) {
            for (uint j = 0; j < PinComboBoxPtrList[i]->pin_types_index.size(); ++j) {
                if (PinComboBoxPtrList[i]->pin_types_index[j] == index)
                {
                    if(PinComboBoxPtrList[i]->interact_count_ == 0){
                        PinComboBoxPtrList[i]->interact_count_+= pin;
                        PinComboBoxPtrList[i]->SetIndex_Iteraction(j, sender_index);
                    }
                    else if (PinComboBoxPtrList[i]->is_interacts_ == true){
                        PinComboBoxPtrList[i]->interact_count_+= pin;
                    }
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < PinComboBoxPtrList.size(); ++i) {
            if (PinComboBoxPtrList[i]->is_interacts_ == true)
            {
                for (size_t j = 0; j < PinComboBoxPtrList[i]->pin_types_index.size(); ++j) {
                    if (PinComboBoxPtrList[i]->pin_types_index[j] == sender_index)
                    {
                        if(PinComboBoxPtrList[i]->interact_count_ > 0){
                            PinComboBoxPtrList[i]->interact_count_-= pin;
                        }
                        if (PinComboBoxPtrList[i]->interact_count_ <= 0) {
                            PinComboBoxPtrList[i]->SetIndex_Iteraction(0, sender_index);
                        }
                    }
                }
            }
        }
    }
}
                                            // GOVNOKOD?

void PinConfig::pinIndexChanged(int current_device_enum, int previous_device_enum, int pin_number) // mutex     // мб сделать сразу запись в конфиг из пинов
{                                                                                                               // или отдельный класс для их состояний
    //fast encoder selected
    if (current_device_enum == FAST_ENCODER){
        emit fastEncoderSelected(PinComboBoxPtrList[0]->pin_list[pin_number - PA_0].gui_name, true);    // hz
    } else if (previous_device_enum == FAST_ENCODER){
        emit fastEncoderSelected(PinComboBoxPtrList[0]->pin_list[pin_number - PA_0].gui_name, false);    // hz
    }
    // shift register latch selected
    if (current_device_enum == SHIFT_REG_LATCH){
        shift_latch_count_++;
        emit shiftRegSelected(pin_number, 0, PinComboBoxPtrList[0]->pin_list[pin_number - PA_0].gui_name);    // hz
    } else if (previous_device_enum == SHIFT_REG_LATCH){
        shift_latch_count_--;
        emit shiftRegSelected((pin_number)*-1, 0, PinComboBoxPtrList[0]->pin_list[pin_number - PA_0].gui_name);    // hz
    }
    // shift register data selected
    if (current_device_enum == SHIFT_REG_DATA){
        shift_data_count_++;
        emit shiftRegSelected(0, pin_number, PinComboBoxPtrList[0]->pin_list[pin_number - PA_0].gui_name);    // hz
    } else if (previous_device_enum == SHIFT_REG_DATA){
        shift_data_count_--;
        emit shiftRegSelected(0, (pin_number)*-1, PinComboBoxPtrList[0]->pin_list[pin_number - PA_0].gui_name);    // hz
    }
    // I2C selected
    if (current_device_enum == I2C_SCL){// || current_device_enum == I2C_SDA){
        emit axesSourceChanged(-2, true);                                            // -2 enum I2C в axes.h
    } else if (previous_device_enum == I2C_SCL){// || previous_device_enum == I2C_SDA){
        emit axesSourceChanged(-2, false);
    }


    // pin type limit           // переизбыток функционала(изи менять в структуре), не думаю, что понадобится в будущем, можно было и захардкодить
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
            for (int j = 0; j < PinComboBoxPtrList.size(); ++j)
            {
                for (size_t k = 0; k < PinComboBoxPtrList[j]->enum_index.size(); ++k) {
                    if (PinComboBoxPtrList[j]->enum_index[k] == pin_type_limit_[i].device_enum_index &&
                        PinComboBoxPtrList[j]->GetCurrentDevEnum() != current_device_enum)
                    {
                        PinComboBoxPtrList[j]->SetIndexStatus(int(k), false);
                    }
                }
            }
        }

        if (limit_is_enable[i] == true && limit_count_array[i] < pin_type_limit_[i].max_count)
        {
            limit_is_enable[i] = false;
            for (int j = 0; j < PinComboBoxPtrList.size(); ++j)
            {
                for (size_t k = 0; k < PinComboBoxPtrList[j]->enum_index.size(); ++k) {
                    if (PinComboBoxPtrList[j]->enum_index[k] == pin_type_limit_[i].device_enum_index)
                    {
                        PinComboBoxPtrList[j]->SetIndexStatus(int(k), true);
                    }
                }
            }
        }
    }


    // set current config and generate signals for another configs
//    else {
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
    //}
}


void PinConfig::ResetAllPins()
{
    for (int i = 0; i < PinComboBoxPtrList.size(); ++i) {
        PinComboBoxPtrList[i]->ResetPin();
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
        ui->text_TotalButtons->setStyleSheet(default_label_style_ + "background-color: rgb(200, 0, 0);");
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
        PinComboBoxPtrList[i]->ReadFromConfig(i);
    }
}

void PinConfig::WriteToConfig(){
    for (uint i = 0; i < pin_count_; ++i) {
        PinComboBoxPtrList[i]->WriteToConfig(i);
    }
}

