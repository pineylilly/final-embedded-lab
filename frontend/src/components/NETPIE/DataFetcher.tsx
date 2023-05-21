import token from "../NETPIE/token.json"

async function getStatus() {
    const data = await fetch('https://api.netpie.io/v2/device/status', {method : 'GET',
        headers : {
            "Authorization" : `${token.auth}`,
        }}).then((response) => response.json())
    console.log(data.status)
    return (data.status == 1) ? true : false
    
}

export { getStatus }