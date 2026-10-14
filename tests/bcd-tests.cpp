#include <QtTest>
#include <QComboBox>
#include <QLabel>
#include <QSettings>
#include <QTemporaryDir>
#include <cstring>
#include "bcdconfig.h"
#include "configtofile.h"
#include "currentconfig.h"
#include "deviceconfig.h"
#include "global.h"
#include "pinconfig.h"
#include "reportconverter.h"

GlobalEnvironment gEnv;

class BcdTests : public QObject
{
    Q_OBJECT
private slots:
    void wireCodes()
    {
        QCOMPARE(int(UART_TX), 31);
        QCOMPARE(int(BCD1_BIT0_GND), 32);
        QCOMPARE(int(BCD4_BIT3_VCC), 63);
        QCOMPARE(sizeof(pin_t), std::size_t(1));
        QVERIFY(!BcdConfig::isBcd(UART_TX));
        QVERIFY(!BcdConfig::isBcd(64));
    }

    void groups_data()
    {
        QTest::addColumn<int>("group");
        QTest::addColumn<int>("bits");
        QTest::addColumn<int>("polarity");
        for (int group = 0; group < 4; ++group)
            for (int bits = 1; bits <= 4; ++bits)
                for (int polarity = 0; polarity < 3; ++polarity)
                    QTest::addRow("group%d-%dbits-polarity%d", group + 1, bits, polarity)
                        << group << bits << polarity;
    }

    void groups()
    {
        QFETCH(int, group);
        QFETCH(int, bits);
        QFETCH(int, polarity);
        pin_t pins[PINS_COUNT]{};
        for (int bit = 0; bit < bits; ++bit)
            // Reverse the pin order; bit significance comes from the mode.
            pins[12 - bit] = pin_t(32 + group * 8 + bit
                                    + (polarity == 1 || (polarity == 2 && bit % 2) ? 4 : 0));
        const auto layout = BcdConfig::inspect(pins, PINS_COUNT);
        QVERIFY(layout.valid());
        QCOMPARE(layout.buttonCount(), 1 << bits);
        QCOMPARE(layout.groups[group].bitCount, bits);
    }

    void invalidGroups()
    {
        pin_t pins[PINS_COUNT]{};
        pins[0] = BCD1_BIT0_GND;
        pins[1] = BCD1_BIT2_VCC;
        auto layout = BcdConfig::inspect(pins, PINS_COUNT);
        QVERIFY(!layout.valid());
        QCOMPARE(layout.buttonCount(), 8); // firmware reserves the highest bit's range
        pins[2] = BCD1_BIT1_GND;
        QVERIFY(BcdConfig::inspect(pins, PINS_COUNT).valid());
        pins[3] = BCD1_BIT0_VCC;
        QVERIFY(!BcdConfig::inspect(pins, PINS_COUNT).valid());
        pins[3] = NOT_USED;
        pins[4] = BCD4_BIT0_VCC;
        QCOMPARE(BcdConfig::inspect(pins, PINS_COUNT).buttonCount(), 10);
    }

    void allModesOnAllPins()
    {
        DeviceConfig device;
        gEnv.pDeviceConfig = &device;
        for (int pin = 0; pin < PINS_COUNT; ++pin) {
            PinComboBox widget(pin + 1);
            auto *combo = widget.findChild<QComboBox *>("comboBox_PinsType");
            QVERIFY(combo);
            for (int mode = 32; mode <= 63; ++mode) {
                const int index = widget.enumIndex().indexOf(mode);
                QVERIFY(index >= 0);
                QVERIFY(!combo->itemData(index, Qt::ToolTipRole).toString().isEmpty());
                combo->setCurrentIndex(index);
                widget.writeToConfig(pin);
                QCOMPARE(int(device.config.pins[pin]), mode);
                combo->setCurrentIndex(0);
                widget.readFromConfig(pin);
                QCOMPARE(widget.currentDevEnum(), mode);
            }
        }
        gEnv.pDeviceConfig = nullptr;
    }

    void guiValidationAndRanges()
    {
        QTemporaryDir dir;
        QSettings settings(dir.filePath("settings.ini"), QSettings::IniFormat);
        DeviceConfig device;
        std::memset(device.config.pins, 0, sizeof(device.config.pins));
        gEnv.pAppSettings = &settings;
        gEnv.pDeviceConfig = &device;
        {
            PinConfig widget;
            QSignalSpy limits(&widget, &PinConfig::limitReached);
            QSignalSpy counts(&widget, &PinConfig::totalButtonsValueChanged);
            auto set = [&](const char *pin, int mode) {
                auto *box = widget.findChild<PinComboBox *>(QLatin1String(pin));
                QVERIFY(box);
                box->findChild<QComboBox *>("comboBox_PinsType")->setCurrentIndex(box->enumIndex().indexOf(mode));
            };
            set("A7", BCD1_BIT2_GND);
            QVERIFY(widget.limitIsReached());
            QVERIFY(widget.validationError().contains("bit0"));
            set("A0", BCD1_BIT0_VCC);
            set("B1", BCD1_BIT1_GND);
            QVERIFY(!widget.limitIsReached());
            QCOMPARE(counts.last().at(0).toInt(), 8);
            set("A1", BCD1_BIT0_GND);
            QVERIFY(widget.validationError().contains("more than once"));
            set("A1", NOT_USED);
            QVERIFY(!widget.limitIsReached());
            set("B0", BUTTON_GND);
            set("A2", BCD3_BIT0_GND);
            auto *summary = widget.findChild<QLabel *>("label_BcdStatus");
            QVERIFY(summary->text().contains("buttons 2–9"));
            QVERIFY(summary->text().contains("buttons 10–11"));
            widget.writeToConfig();
            QCOMPARE(int(device.config.pins[7]), int(BCD1_BIT2_GND));
            widget.resetAllPins();
            widget.readFromConfig();
            QVERIFY(!widget.limitIsReached());
            QCOMPARE(counts.last().at(0).toInt(), 11);
            widget.shiftRegButtonsCountChanged(120);
            QVERIFY(widget.limitIsReached());
            widget.shiftRegButtonsCountChanged(121);
            QVERIFY(widget.limitIsReached()); // repeated overflow must stay blocked
            QCOMPARE(counts.last().at(0).toInt(), 128);
            widget.shiftRegButtonsCountChanged(117);
            QVERIFY(!widget.limitIsReached()); // exactly 128 is allowed
            QVERIFY(!limits.isEmpty());
            QVERIFY(!limits.last().at(0).toBool());
            widget.resetAllPins();
            QVERIFY(!widget.limitIsReached());
        }
        gEnv.pAppSettings = nullptr;
        gEnv.pDeviceConfig = nullptr;
    }

    void combinedLimits()
    {
        CurrentConfig widget;
        widget.bcdButtonsCountChanged(64);
        widget.a2bCountChanged(64);
        QVERIFY(!widget.limitIsReached());
        widget.shiftRegButtonsCountChanged(1);
        QVERIFY(widget.limitIsReached());
        widget.totalLEDsChanged(MAX_LEDS_NUM + 1);
        widget.shiftRegButtonsCountChanged(0);
        QVERIFY(widget.limitIsReached()); // LED overflow remains
        widget.totalLEDsChanged(0);
        QVERIFY(!widget.limitIsReached());
    }

    void fileAndUsbRoundTrip()
    {
        QTemporaryDir dir;
        DeviceConfig original, loaded;
        for (int group = 0; group < 4; ++group)
            for (int bit = 0; bit < 4; ++bit)
                original.config.pins[group * 4 + bit] = pin_t(32 + group * 8 + bit + (bit % 2 ? 4 : 0));
        for (int button = 0; button < 64; ++button) {
            original.config.buttons[button].physical_num = button;
            original.config.buttons[button].type = BUTTON_NORMAL;
        }
        ConfigToFile::saveDeviceConfigToFile(dir.filePath("bcd.cfg"), original.config);
        ConfigToFile::loadDeviceConfigFromFile(nullptr, dir.filePath("bcd.cfg"), loaded.config);
        QCOMPARE(std::memcmp(original.config.pins, loaded.config.pins, sizeof(original.config.pins)), 0);
        QCOMPARE(BcdConfig::inspect(loaded.config.pins, PINS_COUNT).buttonCount(), 64);
        for (int button = 0; button < 64; ++button)
            QCOMPARE(int(loaded.config.buttons[button].physical_num), button);
        gEnv.pDeviceConfig = &original;
        const int reports = (sizeof(dev_config_t) + 61) / 62;
        for (int report = 1; report <= reports; ++report) {
            uint8_t buffer[BUFFERSIZE]{};
            gEnv.pDeviceConfig = &original;
            ReportConverter::sendConfigToDevice(buffer, report);
            gEnv.pDeviceConfig = &loaded;
            ReportConverter::getConfigFromDevice(buffer);
        }
        QCOMPARE(std::memcmp(&original.config, &loaded.config, sizeof(dev_config_t)), 0);
        gEnv.pDeviceConfig = nullptr;
    }
};

QTEST_MAIN(BcdTests)
#include "bcd-tests.moc"
