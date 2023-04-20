import TemperatureGraph from "./Graph/TemperatureGraph"

function Home() {

    return (
        <div className="bg-white dark:bg-gray-900 w-full min-h-screen">
            <TemperatureGraph />
        </div>
    )
}

export default Home