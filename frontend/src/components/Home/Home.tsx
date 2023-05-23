import { useState, useEffect, useRef } from 'react'
import { Route, Routes } from 'react-router-dom'
import TemperatureGraph from "./Graph/TemperatureGraph"
import Panel from "./Panel/Panel"
import { getFeed, getRecentFeed } from '../NETPIE/DataFetcher';
import GraphTimeSwitch from './Panel/GraphTimeSwitch';
import DustGraph from './Graph/DustGraph';
import HumidityGraph from './Graph/HumidityGraph';
import RawDataPanel from './Panel/RawDataPanel';

function Home() {
    const displaySetting: any = {
        "Realtime" : {
            "startValue" : 30,
            "startUnit" : "minutes",
            "samplingValue" : 30,
            "samplingUnit" : "seconds",
            "minuteBound" : 30
        },
        "30 minutes" : {
            "startValue" : 30,
            "startUnit" : "minutes",
            "samplingValue" : 30,
            "samplingUnit" : "seconds",
            "minuteBound" : 30
        },
        "1 hour" : {
            "startValue" : 1,
            "startUnit" : "hours",
            "samplingValue" : 1,
            "samplingUnit" : "minutes",
            "minuteBound" : 60
        },
        "2 hours" : {
            "startValue" : 2,
            "startUnit" : "hours",
            "samplingValue" : 2,
            "samplingUnit" : "minutes",
            "minuteBound" : 120
        },
        "3 hours" : {
            "startValue" : 3,
            "startUnit" : "hours",
            "samplingValue" : 3,
            "samplingUnit" : "minutes",
            "minuteBound" : 180
        },
        "6 hours" : {
            "startValue" : 6,
            "startUnit" : "hours",
            "samplingValue" : 6,
            "samplingUnit" : "minutes",
            "minuteBound" : 360
        },
        "12 hours" : {
            "startValue" : 12,
            "startUnit" : "hours",
            "samplingValue" : 12,
            "samplingUnit" : "minutes",
            "minuteBound" : 720
        },
        "1 day" : {
            "startValue" : 1,
            "startUnit" : "days",
            "samplingValue" : 24,
            "samplingUnit" : "minutes",
            "minuteBound" : 1440
        },
        "2 days" : {
            "startValue" : 2,
            "startUnit" : "days",
            "samplingValue" : 48,
            "samplingUnit" : "minutes",
            "minuteBound" : 2880
        },
        "5 days" : {
            "startValue" : 5,
            "startUnit" : "days",
            "samplingValue" : 120,
            "samplingUnit" : "minutes",
            "minuteBound" : 7200
        }
    }

    const msPerMinute = 60000;

    const dataFetchedRef = useRef(false);
    const timerRef = useRef<any>();
    
    const [dust, setDust] = useState<Array<Array<number>>>([]);
    const [temperature, setTemperature] = useState<Array<Array<number>>>([]);
    const [humidity, setHumidity] = useState<Array<Array<number>>>([]);
    const [timeMin, setTimeMin] = useState<string>();
    const [timeMax, setTimeMax] = useState<string>();

    const [displayMode, setDisplayMode] = useState<string>("Realtime");


    function handleDisplayModeChange(mode: string) {
        if (mode === displayMode) return
        setDisplayMode(mode)
        getFeed(displaySetting[mode]['startValue'], displaySetting[mode]['startUnit'], displaySetting[mode]['samplingValue'], displaySetting[mode]['samplingUnit'])
        .then((res) => {
            console.log(res)
            setDust(res.dust)
            setTemperature(res.temperature)
            setHumidity(res.humidity)
            const g = (new Date(Date.now() - displaySetting[mode]['minuteBound'] * msPerMinute)).toISOString()
            const l = (new Date(Date.now())).toISOString()
            setTimeMin(g.slice(0,g.length - 5) + 'Z')
            setTimeMax(l.slice(0,l.length - 5) + 'Z')
        })

    }

    useEffect(() => {
        // Clear any running intervals on component unmount
        return () => clearInterval(timerRef.current);
      }, []);

    useEffect(() => {
        if (!dataFetchedRef.current) {
            getRecentFeed().then((res) => {
                console.log(res)
                setDust(res.dust)
                setTemperature(res.temperature)
                setHumidity(res.humidity)
                const g = (new Date(Date.now() - 30 * msPerMinute)).toISOString()
                const l = (new Date(Date.now())).toISOString()
                setTimeMin(g.slice(0,g.length - 5) + 'Z')
                setTimeMax(l.slice(0,l.length - 5) + 'Z')
            })
            dataFetchedRef.current = true
            console.log("fetch data")
        }

        if (displayMode === "Realtime") {
            timerRef.current = setInterval(
                () => {
                    getRecentFeed().then((res) => {
                        console.log(res)
                        setDust(res.dust)
                        setTemperature(res.temperature)
                        setHumidity(res.humidity)
                        const g = (new Date(Date.now() - 30 * msPerMinute)).toISOString()
                        const l = (new Date(Date.now())).toISOString()
                        setTimeMin(g.slice(0,g.length - 5) + 'Z')
                        setTimeMax(l.slice(0,l.length - 5) + 'Z')
                    })
                },
                30000
              )
        }
        else {
            clearInterval(timerRef.current)
        }
    }, [displayMode]);

    return (
        <div className="bg-white dark:bg-gray-900 w-full min-h-screen">
            <Panel/>
            <GraphTimeSwitch displayMode={displayMode} handleDisplayMode={handleDisplayModeChange}/>
            <Routes>
                <Route path='/' element={<><DustGraph data={dust} min={timeMin} max={timeMax}/></>} />
                <Route path='/dust' element={<><DustGraph data={dust} min={timeMin} max={timeMax}/></>} />
                <Route path='/temperature' element={<><TemperatureGraph data={temperature}  min={timeMin} max={timeMax}/></>} />
                <Route path='/humidity' element={<><HumidityGraph data={humidity}  min={timeMin} max={timeMax}/></>} />
            </Routes>
            <Routes>
                <Route path='/' element={<><RawDataPanel data={dust} header="Avg Dust (ug/m^3)"/></>} />
                <Route path='/dust' element={<><RawDataPanel data={dust} header="Avg Dust (ug/m^3)"/></>} />
                <Route path='/temperature' element={<><RawDataPanel data={temperature} header="Avg Temperature (°C)"/></>} />
                <Route path='/humidity' element={<><RawDataPanel data={humidity} header="Avg Humidity"/></>} />
            </Routes>
        </div>
    )
}

export default Home