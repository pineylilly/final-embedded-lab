import TemperatureGraph from "./Graph/TemperatureGraph"
import Panel from "./Panel/Panel"

function Home() {

    return (
        <div className="bg-white dark:bg-gray-900 w-full min-h-screen">
            <Panel/>
            <TemperatureGraph />
        </div>
    )
}

export default Home