import { ResponsiveLine } from '@nivo/line'
import temperatureSampleData from "../../../resources/sample/sampletemperature.json"
import downloadjs from 'downloadjs'
import html2canvas from 'html2canvas'

function TemperatureGraph() {

    const handleCaptureClick = async () => {
        const Elem = document.querySelector<HTMLElement>('#temperature-graph')
        if (!Elem) return
    
        const canvas = await html2canvas(Elem)
        const dataURL = canvas.toDataURL('image/png')
        downloadjs(dataURL, 'temperature-graph.png', 'image/png')
    }
    
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
                    data={temperatureSampleData}
                    margin={{ top: 30, right: 30, bottom: 50, left: 60 }}
                    xScale={{
                        type: "time",
                        format: "%Y-%m-%dT%H:%M:%SZ",
                        precision: "second"
                    }}
                    yScale={{
                        type: 'linear',
                        min: 'auto',
                        max: 'auto',
                        stacked: true,
                        reverse: false
                    }}
                    //xFormat="time:%Y-%m-%dT%H:%M:%SZ"
                    yFormat=" >-.2f"
                    axisTop={null}
                    axisRight={null}
                    axisBottom={{  
                        format: '%b %d %H:%M:%S',
                        tickSize: 5,
                        tickPadding: 5,
                        tickRotation: 0,
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