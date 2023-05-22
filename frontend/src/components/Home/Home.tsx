import { useState, useEffect, useRef } from 'react'
import { Route, Routes } from 'react-router-dom'
import TemperatureGraph from "./Graph/TemperatureGraph"
import Panel from "./Panel/Panel"
import { getRecentFeed } from '../NETPIE/DataFetcher';

function Home() {
    const msPerMinute = 60000;

    const dataFetchedRef = useRef(false);
    
    const [dust, setDust] = useState<Array<Array<number>>>([]);
    const [temperature, setTemperature] = useState<Array<Array<number>>>([]);
    const [humidity, setHumidity] = useState<Array<Array<number>>>([]);
    const [timeMin, setTimeMin] = useState<string>();
    const [timeMax, setTimeMax] = useState<string>();

    const [timer, setTimer] = useState(0)

    useEffect(() => {
        if (!dataFetchedRef.current) {
            getRecentFeed().then((res) => {
                console.log(res)
                setDust(res.dust)
                setTemperature(res.temperature)
                setHumidity(res.humidity)
                const g = (new Date(Date.now() - 31 * msPerMinute)).toISOString()
                const l = (new Date(Date.now())).toISOString()
                setTimeMin(g.slice(0,g.length - 5) + 'Z')
                setTimeMax(l.slice(0,l.length - 5) + 'Z')
            })
            dataFetchedRef.current = true
            console.log("fetch data")
        }
    });

    return (
        <div className="bg-white dark:bg-gray-900 w-full min-h-screen">
            <Panel/>
            <Routes>
                <Route path='/' element={<><TemperatureGraph/></>} />
                <Route path='/dust' element={<><TemperatureGraph data={dust} min={timeMin} max={timeMax}/></>} />
                <Route path='/temperature' element={<><TemperatureGraph data={temperature}  min={timeMin} max={timeMax}/></>} />
                <Route path='/humidity' element={<><TemperatureGraph data={humidity}  min={timeMin} max={timeMax}/></>} />
            </Routes>
        </div>
    )
}

export default Home