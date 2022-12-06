import React, { useState } from 'react';
import csv from 'csvtojson';
import { useEffect } from 'react';
import axios from 'axios';
import LoteAreaChart from '../components/LoteAreaChart';
import { useParams } from 'react-router-dom';

export default function ReceitaDetails() {
    const [loading, setLoading] = useState(true);
    const [initDate, setInitDate] = useState(null);
    const [endDate, setEndDate] = useState(null);
    const [sublotes, setSublotes] = useState([]);
    const {id} = useParams();

    useEffect(() => {
        async function fetchData() {
            const response = await axios.get(`/recipes/${id}`);
            let rawData = response.data;
            console.log(rawData);

            csv().fromString(rawData.toString()).then(json => {
                console.log(json);
                let result = parseHistorico(json);

                setInitDate(result.initDate);
                setEndDate(result.endDate);
                setSublotes(result.sublotes);
                setLoading(false);
            });
        }

        fetchData();
    }, []);

    if (loading) {
        return <pre>Loading...</pre>
    }

    return (
        <>
            <h1 className="text-center text-3xl pb-8">Lote {id}</h1>
            {sublotes.map(sublote => (
                <div className='items-center'>
                    <LoteAreaChart sublote={sublote} />
                </div>
            ))}
        </>
    )
}

function parseHistorico(json) {
    let result = {
        initDate: null,
        endDate: null,

        sublotes: []
    }

    let validLote = false;
    let subloteCount = 0;
    for (let i = 0; i < json.length; i++) {
        let entry = json[i];
        let date = Number(entry.date);
        let value = entry.value;

        if (entry.sensor === "LOTE") {
            if (value === "1") {
                result.initDate = date;
                validLote = true;
            } else if (entry.value === "0") {
                result.endDate = date;
                validLote = false;
            }
        }

        if (validLote) {
            if (entry.sensor === "DEVICE_STATE") {
                if (value === "1") {
                    result.sublotes.push({
                        initDate: date,
                        endDate: null,

                        labels: [],
                        sensor_entr: [],
                        sensor_m1: [],
                        sensor_m2: [],
                        sensor_m3: [],
                        sensor_m4: [],
                    })
                } else if (value === "0") {
                    result.sublotes[subloteCount].endDate = date;
                    subloteCount++;
                }

            } else if (entry.sensor === "SENSOR_ENTR") {
                result.sublotes[subloteCount].sensor_entr.push({
                    date: date,
                    value: value
                })

                if (result.sublotes[subloteCount].labels.includes(date) === false)
                    result.sublotes[subloteCount].labels.push(date);
            } else if (entry.sensor === "SENSOR_M1") {
                result.sublotes[subloteCount].sensor_m1.push({
                    date: date,
                    value: value
                })

                if (result.sublotes[subloteCount].labels.includes(date) === false)
                    result.sublotes[subloteCount].labels.push(date);
            } else if (entry.sensor === "SENSOR_M2") {
                result.sublotes[subloteCount].sensor_m2.push({
                    date: date,
                    value: value
                })

                if (result.sublotes[subloteCount].labels.includes(date) === false)
                    result.sublotes[subloteCount].labels.push(date);
            } else if (entry.sensor === "SENSOR_M3") {
                result.sublotes[subloteCount].sensor_m3.push({
                    date: date,
                    value: value
                })

                if (result.sublotes[subloteCount].labels.includes(date) === false)
                    result.sublotes[subloteCount].labels.push(date);
            } else if (entry.sensor === "SENSOR_M4") {
                result.sublotes[subloteCount].sensor_m4.push({
                    date: date,
                    value: value
                })

                if (result.sublotes[subloteCount].labels.includes(date) === false)
                    result.sublotes[subloteCount].labels.push(date);
            }
        }
    }

    return result;
}