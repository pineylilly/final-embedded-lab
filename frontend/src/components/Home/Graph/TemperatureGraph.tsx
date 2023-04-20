import { ResponsiveLine } from '@nivo/line'
import sampleData from "../../../resources/sample/sampledata.json"
import temperatureSampleData from "../../../resources/sample/sampletemperature.json"

function TemperatureGraph() {
    return (
        <div className="h-96">
        <ResponsiveLine 
            data={temperatureSampleData}
            margin={{ top: 50, right: 60, bottom: 50, left: 60 }}
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
    )
}

export default TemperatureGraph;