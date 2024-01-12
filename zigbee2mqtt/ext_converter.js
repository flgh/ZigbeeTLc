const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const tz = require('zigbee-herdsman-converters/converters/toZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const ota = require('zigbee-herdsman-converters/lib/ota');
const e = exposes.presets;
const ea = exposes.access;

const definition = {
        zigbeeModel: ['LYWSD03MMC'],
        model: 'LYWSD03MMC',
        vendor: 'Custom devices (DiY)',
        description: 'Xiaomi temperature, switch & humidity sensor with custom firmware',
        fromZigbee: [fz.temperature, fz.humidity, fz.battery, fz.hvac_user_interface, fz.command_toggle],
        toZigbee: [tz.thermostat_temperature_display_mode],
        configure: async (device, coordinatorEndpoint, logger) => {
            const endpoint = device.getEndpoint(1);
            const bindClusters = ['msTemperatureMeasurement', 'msRelativeHumidity', 'genPowerCfg', 'genOnOff'];
            await reporting.bind(endpoint, coordinatorEndpoint, bindClusters);
            await reporting.temperature(endpoint, {min: 10, max: 300, change: 10});
            await reporting.humidity(endpoint, {min: 10, max: 300, change: 50});
            await reporting.batteryVoltage(endpoint);
            await reporting.batteryPercentageRemaining(endpoint);
        },
        exposes: [
            e.temperature(), e.humidity(), e.battery(), e.action(['toggle']),
            e.enum('temperature_display_mode', ea.ALL, ['celsius', 'fahrenheit'])
                .withDescription('The temperature format displayed on the screen'),
        ],
        ota: ota.zigbeeOTA,
};

module.exports = definition;
