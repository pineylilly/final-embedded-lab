import Collapsible from "../../Asset/Collapsible"

function RawDataPanel (props: {data: Array<Array<number>>, header: string}) {
    const data = props.data.map((e) => {
        const g = (new Date(e[0])).toISOString()
        return [g.slice(0,g.length - 5) + 'Z' , Number(e[1].toFixed(2))]})
    
    return (
        <div className="flex flex-col p-6 md:p-9 md:px-24 w-full border-t  bg-gray-100">
            <Collapsible 
                trigger={"Raw Data"} 
                triggerTagName="div" 
                triggerClassName="py-2 px-4 w-full bg-slate-200 rounded-md text-md"
                triggerOpenedClassName="py-2 px-4 w-full bg-slate-200 rounded-md text-md"
                contentOuterClassName="border-black border-1"
            >
                <div className="py-4 px-4 flex flex-col space-y-4 rounded-md">
                    <table className="rounded-md">
                        <thead>
                            <tr>
                                <th>Time (UTC)</th>
                                <th>{props.header}</th>
                            </tr>
                        </thead>
                        <tbody>
                        {
                            data.map((e) => {return (
                            <tr>
                                <td>{e[0]}</td>
                                <td>{e[1]}</td>
                            </tr>)})
                        }
                        </tbody>
                    </table>
                </div>
                
            </Collapsible>
        </div>
    )
}

export default RawDataPanel