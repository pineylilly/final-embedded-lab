import { useState, useEffect } from 'react'
import { ResponsiveLine } from '@nivo/line'
import temperatureSampleData from "../../../resources/sample/sampletemperature.json"
import downloadjs from 'downloadjs'
import html2canvas from 'html2canvas'

function TemperatureGraph(props: { data?: Array<Array<number>>, min?: string, max?: string }) {
    var result: any = temperatureSampleData
    if (props.data) {
        result = [{'id' : "temperature",
        "color": "hsl(176, 70%, 50%)", 
        "data" : props.data.map( (element: any) => {
            const g = (new Date(element[0])).toISOString()
            return {"x" : g.slice(0,g.length - 5) + 'Z' , "y" : Number(element[1].toFixed(2))}})
        }]
    }
    
    console.log(props.data)
    const [tickValues, setTickValues] = useState<number>((window.innerWidth <= 760) ? 3 : 7);


    const handleCaptureClick = async () => {
        const Elem = document.querySelector<HTMLElement>('#temperature-graph')
        if (!Elem) return
    
        const canvas = await html2canvas(Elem)
        const dataURL = canvas.toDataURL('image/png')
        downloadjs(dataURL, 'temperature-graph.png', 'image/png')
    }

    function handleWindowSizeChange() {
        setTickValues((window.innerWidth <= 760) ? 3 : 7);
    }

    useEffect(() => {
        window.addEventListener('resize', handleWindowSizeChange);
        return () => {
            window.removeEventListener('resize', handleWindowSizeChange);
        }
    }, []);
    
    return (
        <div className="px-1 py-4">
            <div className="flex flex-row pl-12 items-center">
                <div className="w-36 h-max">
                    <h2 className="text-xl align-middle">Temperature</h2>
                </div>
                
                <button className='normal-button' onClick={handleCaptureClick}>Download</button>
            </div>
            <div className="h-96" id="temperature-graph">
                <ResponsiveLine 
                    data={result}
                    margin={{ top: 30, right: 30, bottom: 50, left: 60 }}
                    xScale={{
                        type: "time",
                        format: "%Y-%m-%dT%H:%M:%SZ",
                        precision: "second",
                        min: props.min || 'auto',
                        max: props.max || 'auto'
                    }}
                    yScale={{
                        type: 'linear',
                        min: 'auto',
                        max: 'auto',
                        stacked: true,
                        reverse: false
                    }}
                    xFormat="time:%d/%m/%y %H:%M:%S"
                    yFormat=" >-.2f"
                    axisTop={null}
                    axisRight={null}
                    axisBottom={{  
                        format: '%b %d %H:%M:%S',
                        tickSize: 5,
                        tickPadding: 5,
                        tickRotation: 0,
                        tickValues: tickValues,
                        legend: 'Time',
                        legendOffset: 36,
                        legendPosition: 'middle'
                    }}
                    axisLeft={{
                        tickSize: 5,
                        tickPadding: 5,
                        tickRotation: 0,
                        legend: 'Temperature (°C)',
                        legendOffset: -40,
                        legendPosition: 'middle'
                    }}
                    colors={['#61cdbb']}
                    pointSize={5}
                    pointColor={{ from: 'color', modifiers: [] }}
                    pointBorderWidth={2}
                    pointBorderColor={{ from: 'serieColor' }}
                    pointLabelYOffset={-12}
                    enableArea={true}
                    useMesh={true}
                    legends={[]}
                />
            </div>
            
        </div>
        
    )
}

export default TemperatureGraph;