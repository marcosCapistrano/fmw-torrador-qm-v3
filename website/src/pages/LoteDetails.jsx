import React, { useState } from 'react';
import csv from 'csvtojson';
import { useEffect } from 'react';
import axios from 'axios';
import LoteAreaChart from '../components/LoteAreaChart';
import { useParams } from 'react-router-dom';

/*
*/

export default function LoteDetails() {
    const [loading, setLoading] = useState(true);
    const [data, setData] = useState({});
    const { id } = useParams();

    useEffect(() => {
        async function fetchData() {
            const response = await axios.get(`/roasts/${id}`);

            let rawData = "time, event, value\n" + response.data;
            console.log(rawData);

            csv().fromString(rawData.toString()).then(json => {
                console.log(json);
                let result = parseHistorico(json);
                setData(result);
                setLoading(false);
            });

            // csv().fromString(rawData.toString()).then(json => {
            //     console.log(json);
            //     let result = parseHistorico(json);

            //     setInitDate(result.initDate);
            //     setEndDate(result.endDate);
            //     setSublotes(result.sublotes);
            //     setLoading(false);
            // });
        }

        fetchData();
    }, []);

    if (loading) {
        return <pre>Loading...</pre>
    }

    return (
        <>
            <h1 className="text-center text-3xl pb-8">Lote {id}</h1>
            <div className='items-center'>
                <LoteAreaChart data2={data} />
            </div>
        </>
    )
}

function fmtMSS(s){return(s-(s%=60))/60+(9<s?':':':0')+s}

function parseHistorico(json) {
    let data = {
        timeArr: [],
        arTempArr: [],
        graoTempArr: [],
        arDeltaArr: [],
        graoDeltaArr: [],
        gasArr: [],
    }

    for (let i = 0; i < json.length; i++) {
        let entry = json[i];
        let time = Number(entry.time);
        let event = entry.event;

        //se array de time não possuir o tempo daquele comando, apenas adiciona ele
        if (event == "SENSOR_AR" || event == "SENSOR_GRAO" || event == "GAS")
            if (data.timeArr.findIndex(el => el == time) == -1) {
                data.timeArr.push(time);
            }
    }

    for (let currTime of data.timeArr) {
        let foundArTemp = false;
        let foundGraoTemp = false;
        let foundGas = false;
        for (let entry of json) {
            if (entry.time == currTime) {
                if (entry.event == "SENSOR_AR") {
                    data.arTempArr.push(entry.value);
                    foundArTemp = true;
                } else if (entry.event == "SENSOR_GRAO") {
                    data.graoTempArr.push(entry.value);
                    foundGraoTemp = true;
                } else if (entry.event == "GAS") {
                    data.gasArr.push(entry.value);
                    foundGas = true;
                }
            }
        }

        if (!foundArTemp) {
            if (data.arTempArr.length > 0)
                data.arTempArr.push(data.arTempArr[data.arTempArr.length - 1]);
            else
                data.arTempArr.push(0);
        }
        if (!foundGraoTemp) {
            if (data.graoTempArr.length > 0)
                data.graoTempArr.push(data.graoTempArr[data.graoTempArr.length - 1]);
            else
                data.graoTempArr.push(0)
        }
        if (!foundGas) {
            if (data.gasArr.length > 0)
                data.gasArr.push(data.gasArr[data.gasArr.length - 1]);
            else
                data.gasArr.push(0)
        }
    }

    for (let i = 0; i < data.arTempArr.length; i++) {
        let next = data.arTempArr[i];
        if (i > 0) {
            let prev = data.arTempArr[i - 1];
            data.arDeltaArr.push((next - prev)*10);
        } else {
            data.arDeltaArr.push(0);
        }
    }

    for (let i = 0; i < data.graoTempArr.length; i++) {
        let next = data.graoTempArr[i];
        if (i > 0) {
            let prev = data.graoTempArr[i - 1];
            data.graoDeltaArr.push((next - prev)*10);
        } else {
            data.graoDeltaArr.push(0);
        }
    }

    return data;
}