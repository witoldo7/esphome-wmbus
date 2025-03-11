/*
 Copyright (C) 2019-2022 Fredrik Öhrström (gpl-3.0-or-later)

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include"meters_common_implementation.h"

namespace
{
    struct Driver : public virtual MeterCommonImplementation
    {
        Driver(MeterInfo &mi, DriverInfo &di);
    };

    static bool ok = registerDriver([](DriverInfo&di)
    {
        di.setName("amiplus");
        di.setDefaultFields("name,id,total_energy_consumption,total_energy_consumption_tariff_1,total_energy_production,total_energy_production_tariff_1,today_energy_consumption,total_energy_consumption_tariff_1,total_energy_production,total_energy_production_tariff_1,today_energy_consumption,today_energy_consumption_tariff_1,today_energy_production,today_energy_production_tariff_1,current_power_consumption,current_power_production,current_rective_current,current_rective_power,voltage_at_phase_1,voltage_at_phase_2,voltage_at_phase_3,max_power_consumption,max_power_production,current_at_phase_1,current_at_phase_2,current_at_phase_3,timestamp");
        di.setMeterType(MeterType::ElectricityMeter);
        di.addLinkMode(LinkMode::T1);
        di.addDetection(MANUFACTURER_APA,  0x02,  0x02);
        di.addDetection(MANUFACTURER_DEV,  0x37,  0x02);
        di.addDetection(MANUFACTURER_DEV,  0x02,  0x00);
        // Apator Otus 1/3 seems to use both, depending on a frame.
        // Frames with APA are successfully decoded by this driver
        // Frames with APT are not - and their content is unknown - perhaps it broadcasts two data formats?
        di.addDetection(MANUFACTURER_APA,  0x02,  0x01);
        di.addDetection(0x14ed,  0x02,  0x01);
        di.setConstructor([](MeterInfo& mi, DriverInfo& di){ return shared_ptr<Meter>(new Driver(mi, di)); });
    });

    Driver::Driver(MeterInfo &mi, DriverInfo &di) : MeterCommonImplementation(mi, di)
    {
        addStringFieldWithExtractor(
            "device_date_time",
            "Device date time.",
            DEFAULT_PRINT_PROPERTIES,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::DateTime)
            );

        addNumericFieldWithExtractor(
            "total_energy_consumption",
            "The total energy consumption recorded by this meter.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::AnyEnergyVIF)
            );

        addNumericFieldWithExtractor(
            "total_energy_consumption_tariff_1",
            "The total energy consumption recorded by this meter on tariff 1.",
            DEFAULT_PRINT_PROPERTIES, // ,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::AnyEnergyVIF)
            .set(TariffNr(1))
            );

        addNumericFieldWithExtractor(
            "total_energy_production",
            "The total energy production recorded by this meter.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0E833C"))
            );

        addNumericFieldWithExtractor(
            "total_energy_production_tariff_1",
            "The total energy production recorded by this meter on tariff 1.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("8E10833C"))
            );

        addNumericFieldWithExtractor(
            "today_energy_consumption",
            "dzisiejsza konsumkcja ?",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0EFB8273"))
            );

        addNumericFieldWithExtractor(
            "today_energy_consumption_tariff_1",
            "dzisiejsza konsumkcja taryfa 1?",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("8E10FB8273"))
            );

        addNumericFieldWithExtractor(
            "total_energy_consumption",
            "Moc czynna pobrana",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0EFB82F33C"))
            );

        addNumericFieldWithExtractor(
            "today_energy_production_tariff_1",
            "dzisiejsza produkcja taryfa 1?",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("8E10FB82F33C"))
            );

        addNumericFieldWithExtractor(
            "current_power_consumption",
            "Current power consumption.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Power,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::PowerW)
            );

        addNumericFieldWithExtractor(
            "current_power_production",
            "Current power production.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Power,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0BAB3C"))
            );

        addNumericFieldWithExtractor(
            "current_rective_power_l",
            "Current ractive power (L).",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Reactive_Power,
            VifScaling::None, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0BFB14")),
            Unit::VAR
            );

        addNumericFieldWithExtractor(
            "current_rective_power_c",
            "Current ractive power (C).",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Reactive_Power,
            VifScaling::None, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0BFB943C")),
            Unit::VAR
            );

        addNumericFieldWithExtractor(
            "voltage_at_phase_1",
            "Voltage at phase L1.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Voltage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0AFDC8FC01"))
            );

        addNumericFieldWithExtractor(
            "voltage_at_phase_2",
            "Voltage at phase L2.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Voltage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0AFDC8FC02"))
            );

        addNumericFieldWithExtractor(
            "voltage_at_phase_3",
            "Voltage at phase L3.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Voltage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0AFDC8FC03"))
            );

        addStringFieldWithExtractor(
            "device_date_time_1",
            "Device date time 1.",
            DEFAULT_PRINT_PROPERTIES,
            FieldMatcher::build()
            .set(DifVifKey("146D"))
            );

        addStringFieldWithExtractor(
            "device_date_time",
            "Device date time.",
            DEFAULT_PRINT_PROPERTIES,
            FieldMatcher::build()
            .set(DifVifKey("14ED3C"))
            );

        addNumericFieldWithExtractor(
            "current_at_phase_1",
            "Instantaneous current in the L1 phase.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Amperage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0BFDDAFC01"))
            );

        addNumericFieldWithExtractor(
            "current_at_phase_2",
            "Instantaneous current in the L2 phase.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Amperage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::Amperage)
            .set(DifVifKey("0BFDDAFC02"))
            );

        addNumericFieldWithExtractor(
            "current_at_phase_3",
            "Instantaneous current in the L3 phase.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Amperage,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(DifVifKey("0BFDDAFC03"))
            );

    }
}
