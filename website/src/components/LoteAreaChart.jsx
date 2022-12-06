import {
    Chart as ChartJS,
    CategoryScale,
    LinearScale,
    PointElement,
    LineElement,
    Title,
    Tooltip,
    Filler,
    Legend,
    LogarithmicScale,
    TimeScale,
    TimeSeriesScale
} from 'chart.js';
import { Line, getDatasetAtEvent, Chart } from 'react-chartjs-2';
import { useState, useEffect, useRef } from 'react';

ChartJS.register(
    CategoryScale,
    LinearScale,
    PointElement,
    LineElement,
    Title,
    Tooltip,
    Filler,
    Legend
);

function createDataset(data, chart) {
    return {
        labels: data.timeArr,
        datasets: [
            {
                label: 'Sensor AR',
                data: data.arTempArr,
                borderColor: 'rgba(3, 227, 252, 0.5)',
                backgroundColor: 'rgba(3, 227, 252, 0.1)',
                yAxisID: 'y',
            },
            {
                label: 'Sensor Grao',
                data: data.graoTempArr,
                borderColor: 'rgba(255, 210, 87, 0.5)',
                backgroundColor: 'rgba(255, 210, 87, 0.1)',
                yAxisID: 'y',
            },
            {
                label: 'Delta Ar',
                data: data.arDeltaArr,
                borderColor: 'rgba(3, 140, 252, 0.5)',
                backgroundColor: 'rgba(3, 140, 252, 0.1)',
                yAxisID: 'y1',
            },
            {
                label: 'Delta Grao',
                data: data.graoDeltaArr,
                borderColor: 'rgba(230, 174, 23, 0.5)',
                backgroundColor: 'rgba(230, 174, 23, 0.1)',
                yAxisID: 'y1',
            },
            {
                label: 'Gas',
                data: data.gasArr,
                borderColor: 'rgba(23, 23, 23, 0.5)',
                backgroundColor: 'rgba(23, 23, 23, 0.1)',
                yAxisID: 'y1',
            },
        ]
    }
}

export default function LoteAreaChart({ data2 }) {
    const [data, setData] = useState({ datasets: [] });
    const [options, setOptions] = useState({
        responsive: true,
        plugins: {
            legend: {
                position: 'top',
            },
            title: {
                display: false,
                text: "",
            },
        },
    });

    const chartRef = useRef(null);

    useEffect(() => {

        const chart = chartRef.current;

        if (!chart)
            return;

        ChartJS.register(LogarithmicScale);
        ChartJS.register(TimeSeriesScale);
        setData(createDataset(data2, chart));
        setOptions(
            {
                ...options,
                scales: {
                    y: {
                        type: 'linear',
                        display: true,
                        position: 'right',
                        // stacked: true,
                    },
                    y1: {
                        type: 'linear',
                        display: true,
                        position: 'left',
                        // stacked: true,
                    },
                }
            }
        )
        // setOptions({...options, plugins: { ...options.plugins, title: { display: true, text: "oi"}}});
    }, []);

    return (
        <div className='w-2/3 h-full mx-auto py-12 text-center'>
            <Line ref={chartRef} options={options} data={data} />
        </div>
    );
}

function createGradientStroke(ctx, area) {
    const colorStart = "rgba(0, 0, 255, 0.5)";
    const colorMid = "rgba(0, 255, 0, 0.5)";
    const colorEnd = "rgba(255, 0, 0, 0.5)";

    const gradient = ctx.createLinearGradient(0, area.bottom, 0, area.top);

    gradient.addColorStop(0, colorStart);
    gradient.addColorStop(0.5, colorMid);
    gradient.addColorStop(1, colorEnd);

    return gradient;
}

function createGradientBackground(ctx, area) {
    const colorStart = "rgba(0, 0, 255, 0.2)";
    const colorMid = "rgba(0, 255, 0, 0.2)";
    const colorEnd = "rgba(255, 0, 0, 0.2)";

    const gradient = ctx.createLinearGradient(0, area.bottom, 0, area.top);

    gradient.addColorStop(0, colorStart);
    gradient.addColorStop(0.5, colorMid);
    gradient.addColorStop(1, colorEnd);

    return gradient;
}