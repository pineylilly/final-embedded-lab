import token from "../NETPIE/token.json"

async function getStatus() {
    const data = await fetch('https://api.netpie.io/v2/device/status', {method : 'GET',
        headers : {
            "Authorization" : `${token.auth}`,
        }}).then((response) => response.json())
    console.log(data.status)
    return (data.status === 1) ? true : false
    
}

async function getRecentData() {
    const data = await fetch('https://api.netpie.io/v2/device/shadow/data', {method : 'GET',
        headers : {
            "Authorization" : `${token.auth}`,
        }}).then((response) => response.json())
    console.log(data.data)
    return data.data
}


export { getStatus }