import { useState, useEffect } from 'react'
import { ResponsiveLine } from '@nivo/line'
import temperatureSampleData from "../../../resources/sample/sampletemperature.json"
import downloadjs from 'downloadjs'
import html2canvas from 'html2canvas'

function DustGraph(props: { data?: Array<Array<number>>, min?: string, max?: string }) {
    var result: any = temperatureSampleData
    if (props.data) {
        result = [{'id' : "dust",
        "color": "hsl(176, 70%, 50%)", 
        "data" : props.data.map( (element: any) => {
            const g = (new Date(element[0])).toISOString()
            return {"x" : g.slice(0,g.length - 5) + 'Z' , "y" : Number(element[1].toFixed(2))}})
        }]
    }
    
    console.log(props.data)
    const [tickValues, setTickValues] = useState<number>((window.innerWidth <= 760) ? 3 : 7);


    const handleCaptureClick = async () => {
        const Elem = document.querySelector<HTMLElement>('#dust-graph')
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
                <div className="pl-3 pr-5 h-max">
                    <h2 className="text-xl align-middle">Dust</h2>
                </div>
                
                <button className='normal-button flex flex-row items-center' onClick={handleCaptureClick}>
                    <svg width="20" height="20" className="ml-[-0.5rem] mr-2">
                        <path d="M16.59 9H15V4c0-.55-.45-1-1-1h-4c-.55 0-1 .45-1 1v5H7.41c-.89 0-1.34 1.08-.71 1.71l4.59 4.59c.39.39 1.02.39 1.41 0l4.59-4.59c.63-.63.19-1.71-.7-1.71zM5 19c0 .55.45 1 1 1h12c.55 0 1-.45 1-1s-.45-1-1-1H6c-.55 0-1 .45-1 1z"></path>
                    </svg>
                    PNG
                </button>

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
                        legend: 'Density (ug/m^3)',
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

export default DustGraph;