import { useState, useEffect, useRef } from 'react'
import Collapsible from "../../Asset/Collapsible"
import Swal from 'sweetalert2'

import token from "../../NETPIE/token.json"

function Panel() {

    const checkboxStyle = "w-11 h-6 bg-gray-200 peer-focus:outline-none rounded-full peer dark:bg-gray-700 peer-checked:after:translate-x-full peer-checked:after:border-white after:content-[''] after:absolute after:top-[2px] after:left-[2px] after:bg-white after:border-gray-300 after:border after:rounded-full after:h-5 after:w-5 after:transition-all dark:border-gray-600 peer-checked:bg-teal-700"

    const dataFetchedRef = useRef(false);
    
    const [autoMode, setAutoMode] = useState<number>(0)
    const [pwmFan, setPwmFan] = useState<number>(0)
    const [pwmWater, setPwmWater] = useState<number>(0)

    const [currentStatus, setCurrentStatus] = useState<string>("")

    function setStatus(a: number, b: number, c: number) {   // a : autoMode, b : pwmFan, c : pwmWater
        // if (autoMode === 1) {
        //     setCurrentStatus("Automatic")
        // }
        // else {
        //     var str = "Manual with " + pwmFan + "% fan and water "
        //     if (pwmWater === 1) {
        //         str += "on"
        //     }
        //     else {
        //         str += "off"
        //     }
        //     setCurrentStatus(str)
        // }
        if (a === 1) {
            setCurrentStatus("Automatic")
        }
        else {
            var str = "Manual with " + b + "% fan and water "
            if (c === 1) {
                str += "on"
            }
            else {
                str += "off"
            }
            setCurrentStatus(str)
        }
    }

    function submitData() {
        // Check condition
        if (autoMode === 0 && !(pwmFan === 0 || (pwmFan >= 50 && pwmFan <= 100))) {
            console.log("PWM Fan value is not okay")
            Swal.fire('Fan speed does not match requirement')
            return
        }
        if (autoMode === 0 && pwmWater === 1 && !(pwmFan >= 50 && pwmFan <= 100)) {
            console.log("Fan must be on when activate water")
            Swal.fire('Fan must be on when activate water')
            return
        }

        const dataSender = `{"data" : {"autoMode" : ${autoMode},"pwmFan" : ${(autoMode === 0) ? pwmFan : 0} , "pwmWater" : ${(autoMode === 0) ? pwmWater : 0}}}`
        console.log(dataSender)

        fetch("https://api.netpie.io/v2/device/message/state", {method : 'PUT',
            headers : {
                "Content-Type": 'application/json',
                "Authorization" : `${token.auth}`,
            },
            body: dataSender}
        ).then((req) => {
            console.log("success")
        })
        .catch((err) => console.log(err))

        fetch("https://api.netpie.io/v2/device/shadow/data", {method : 'PUT',
            headers : {
                "Content-Type": 'application/json',
                "Authorization" : `${token.auth}`,
            },
            body: dataSender}
        ).then((req) => {
            console.log("write shadow success")
        })
        .catch((err) => console.log(err))

        Swal.fire('Device setting saving success')
        
        setStatus(autoMode, pwmFan, pwmWater)



    }

    useEffect(() => {
        if (!dataFetchedRef.current) {
            fetch('https://api.netpie.io/v2/device/shadow/data', {method : 'GET',
            headers : {
                "Authorization" : `${token.auth}`,
            }}).then((response) => response.json())
            .then((jsonData) => {
                setAutoMode(jsonData.data.autoMode)
                setPwmFan(jsonData.data.pwmFan)
                setPwmWater(jsonData.data.pwmWater)
                setStatus(jsonData.data.autoMode, jsonData.data.pwmFan, jsonData.data.pwmWater)
            })
            
            dataFetchedRef.current = true
            console.log("get recent mode")
        }
    });

    return (
        <div className="flex flex-col p-6 md:p-9 md:px-24 w-full">
            <Collapsible 
                trigger={"Device Setting : " + currentStatus} 
                triggerTagName="div" 
                triggerClassName="py-2 px-4 w-full bg-slate-200 rounded-md text-md"
                triggerOpenedClassName="py-2 px-4 w-full bg-slate-200 rounded-md text-md"
                contentOuterClassName="border-black border-1"
            >
                <div className="py-4 px-4 flex flex-col space-y-4">
                    <label className="relative inline-flex items-center cursor-pointer">
                        <input type="checkbox" checked={(autoMode === 1) ? true : false} className="sr-only peer" onChange={() => {setAutoMode((p) => 1-p)}}/>
                        <div className={checkboxStyle}></div>
                        <span className="ml-3 text-sm font-medium text-black dark:text-gray-300">Automatic Mode</span>
                    </label>
                    {
                        (autoMode === 0) && (
                        <>
                            <label className="relative inline-flex items-center cursor-pointer">
                                <input type="text" value={pwmFan} pattern="[0-9]*" className="w-16 border rounded-lg px-2" onChange={(event) => {setPwmFan(parseInt(event.target.value, 10) || 0)}}/>
                                <span className="ml-3 text-sm font-medium text-black dark:text-gray-300">Adjust Fan (%) [0 for off, 50-100 for on]</span>
                            </label>
                            <label className="relative inline-flex items-center cursor-pointer">
                                <input type="checkbox" checked={(pwmWater === 1) ? true : false} className="sr-only peer" onChange={() => {setPwmWater((p) => 1-p)}}/>
                                <div className={checkboxStyle}></div>
                                <span className="ml-3 text-sm font-medium text-black dark:text-gray-300">Activate Water [Fan must be on]</span>
                            </label>
                        </>
                        )
                    }
                    <button className='normal-button' onClick={submitData}>Save</button>
                </div>
                
            </Collapsible>
        </div>
    )




}

export default Panel