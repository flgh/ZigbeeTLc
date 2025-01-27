// Autor: Markus Bodengriller https://github.com/Bodengriller
// requires zigbee2mqtt v1.34+
// external converter for Xiaomi LYWSD03MMC https://github.com/pvvx/ZigbeeTLc
// based on external converter for devbis-Firmware
// https://raw.githubusercontent.com/devbis/z03mmc/master/converters/lywsd03mmc.js

const {
    batteryPercentage,
    temperature,
    humidity,
    enumLookup,
    binary,
    numeric,
    quirkAddEndpointCluster,
} = require('zigbee-herdsman-converters/lib/modernExtend');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const ota = require('zigbee-herdsman-converters/lib/ota');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const e = exposes.presets;
const fz = require('zigbee-herdsman-converters/converters/fromZigbee');

const dataType = {
    boolean: 0x10,
    uint8: 0x20,
    uint16: 0x21,
    int8: 0x28,
    int16: 0x29,
    enum8: 0x30,
};

const definition = {
    zigbeeModel: ['LYWSD03MMC-z'],
    model: 'LYWSD03MMC',
    vendor: 'Xiaomi',
    description: 'Temperature, humidity & switch sensor with custom firmware',
    endpoint: (device) => { return {'sensor': 0x01, 'switch': 0x02, }},
    extend: [
        quirkAddEndpointCluster({
            endpointID: 1,
            outputClusters: [],
            inputClusters: [
                'genPowerCfg',
                'msTemperatureMeasurement',
                'msRelativeHumidity',
                'hvacUserInterfaceCfg',
            ],
        }),
        quirkAddEndpointCluster({
            endpointID: 2,
            outputClusters: ['genOnOff'],
            inputClusters: [],
        }),
        batteryPercentage(),
        temperature({reporting: {min: 10, max: 300, change: 10}}),
        humidity({reporting: {min: 10, max: 300, change: 50}}),
        enumLookup({
            name: 'temperature_display_mode',
            lookup: {'celsius': 0, 'fahrenheit': 1},
            cluster: 'hvacUserInterfaceCfg',
            attribute: {ID: 0x0000, type: dataType.enum8},
            description: 'The units of the temperature displayed on the device screen.',
        }),
        binary({
            name: 'show_smiley',
            valueOn: ['SHOW', 0],
            valueOff: ['HIDE', 1],
            cluster: 'hvacUserInterfaceCfg',
            attribute: {ID: 0x0002, type: dataType.enum8},
            description: 'Whether to show a smiley on the device screen.',
        }),
        numeric({
            name: 'temperature_calibration',
            unit: 'ºC',
            cluster: 'hvacUserInterfaceCfg',
            attribute: {ID: 0x0100, type: dataType.int8},
            valueMin: -12.7,
            valueMax: 12.7,
            valueStep: 0.1,
            scale: 10,
            description: 'The temperature calibration, in 0.1° steps.',
        }),
        numeric({
            name: 'humidity_calibration',
            unit: '%',
            cluster: 'hvacUserInterfaceCfg',
            attribute: {ID: 0x0101, type: dataType.int8},
            valueMin: -12.7,
            valueMax: 12.7,
            valueStep: 0.1,
            scale: 10,
            description: 'The humidity offset is set in 0.1 % steps.',
        }),
        numeric({
            name: 'comfort_temperature_min',
            unit: 'ºC',
            cluster: 'hvacUserInterfaceCfg',
            attribute: {ID: 0x0102, type: dataType.int8},
            valueMin: -127.0,
            valueMax: 127.0,
            description: 'Comfort parameters/Temperature minimum, in 1°C steps.',
        }),
        numeric({
            name: 'comfort_temperature_max',
            unit: 'ºC',
            cluster: 'hvacUserInterfaceCfg',
            attribute: {ID: 0x0103, type: dataType.int8},
            valueMin: -127.0,
            valueMax: 127.0,
            description: 'Comfort parameters/Temperature maximum, in 1°C steps.',
        }),
        numeric({
            name: 'comfort_humidity_min',
            unit: '%',
            cluster: 'hvacUserInterfaceCfg',
            attribute: {ID: 0x0104, type: dataType.uint8},
            valueMin: 0.0,
            valueMax: 100.0,
            description: 'Comfort parameters/Humidity minimum, in 1% steps.',
        }),
        numeric({
            name: 'comfort_humidity_max',
            unit: '%',
            cluster: 'hvacUserInterfaceCfg',
            attribute: {ID: 0x0105, type: dataType.uint8},
            valueMin: 0.0,
            valueMax: 100.0,
            description: 'Comfort parameters/Humidity maximum, in 1% steps.',
        }),
    ],
    ota: ota.zigbeeOTA,
    meta: {multiEndpoint: true},
    configure: async (device, coordinatorEndpoint, logger) => {
        const endpoint1 = device.getEndpoint(1);
	const endpoint2 = device.getEndpoint(2);
        const bindClusters1 = ['msTemperatureMeasurement', 'msRelativeHumidity', 'genPowerCfg'];
        const bindClusters2 = ['genOnOff'];
        await reporting.bind(endpoint1, coordinatorEndpoint, bindClusters1);
	await reporting.bind(endpoint2, coordinatorEndpoint, bindClusters2);
        await reporting.temperature(endpoint1, {min: 10, max: 300, change: 10});
        await reporting.humidity(endpoint1, {min: 10, max: 300, change: 50});
        await reporting.batteryPercentageRemaining(endpoint1);
        try {
            await endpoint1.read('hvacThermostat', [0x0010, 0x0011, 0x0102, 0x0103, 0x0104, 0x0105]);
            await endpoint1.read('msTemperatureMeasurement', [0x0010]);
            await endpoint1.read('msRelativeHumidity', [0x0010]);
        } catch (e) {
            /* backward compatibility */
        }
    },
    fromZigbee: [fz.command_toggle],
    exposes: [e.action(['toggle'])],
};

module.exports = definition;
