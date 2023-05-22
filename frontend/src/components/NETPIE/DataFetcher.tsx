import token from "../NETPIE/token.json"


async function getStatus() {
    const data = await fetch('https://api.netpie.io/v2/device/status', {method : 'GET',
        headers : {
            "Authorization" : `${token.auth}`,
        }}).then((response) => response.json())
    console.log(data.status)
    return (data.status === 1) ? true : false
    
}

async function getRecentFeed() {
    const reqBody = {
        "start_relative": { "value": 30, "unit":"minutes" },
        "metrics": [
            {
                "name": token.deviceID,
                "tags": {"attr":["dust","temperature","humidity"]},
                "limit": 1000,
                "group_by": [{ "name":"tag", "tags":["attr"] }],
                "aggregators": [
                    {
                        "name":"avg",
                        "sampling": {
                            "value":"30",
                            "unit":"seconds"
                        }
                    }
                ]
                    
            }
        ]
    }

    const response = await fetch("https://ds.netpie.io/feed/api/v1/datapoints/query", {method : 'POST',
          headers : {
            "Content-Type": 'application/json',
            "Authorization" : `${token.auth}`,
          },
          body: JSON.stringify(reqBody)}
        ).then((res) => res.json())
    console.log(response)
    var dust: any[] = []
    var temperature: any[] = []
    var humidity: any[] = []

    if (response["queries"][0]["results"].length > 1) {
        for (var i = 0; i < 3; i++) {
            if (response["queries"][0]["results"][i]["tags"]["attr"][0] === "dust") {
                dust = response["queries"][0]["results"][i]["values"] || []
            }
            else if (response["queries"][0]["results"][i]["tags"]["attr"][0] === "temperature") {
                temperature = response["queries"][0]["results"][i]["values"] || []
            }
            else if (response["queries"][0]["results"][i]["tags"]["attr"][0] === "humidity") {
                humidity = response["queries"][0]["results"][i]["values"] || []
            }
        }
    }
    return {
        dust: dust,
        temperature: temperature,
        humidity: humidity
    };
}

async function getFeed(startValue: number, startUnit: string, samplingValue: number, samplingUnit: string) {
    const reqBody = {
        "start_relative": { "value": String(startValue), "unit":startUnit },
        "metrics": [
            {
                "name": token.deviceID,
                "tags": {"attr":["dust","temperature","humidity"]},
                "limit": 1000,
                "group_by": [{ "name":"tag", "tags":["attr"] }],
                "aggregators": [
                    {
                        "name":"avg",
                        "sampling": {
                            "value": String(samplingValue),
                            "unit": samplingUnit
                        }
                    }
                ]
                    
            }
        ]
    }

    console.log(reqBody)

    const response = await fetch("https://ds.netpie.io/feed/api/v1/datapoints/query", {method : 'POST',
          headers : {
            "Content-Type": 'application/json',
            "Authorization" : `${token.auth}`,
          },
          body: JSON.stringify(reqBody)}
        ).then((res) => res.json())
    var dust: any[] = []
    var temperature: any[] = []
    var humidity: any[] = []

    if (response["queries"][0]["results"].length > 1) {
        for (var i = 0; i < 3; i++) {
            if (response["queries"][0]["results"][i]["tags"]["attr"][0] === "dust") {
                dust = response["queries"][0]["results"][i]["values"] || []
            }
            else if (response["queries"][0]["results"][i]["tags"]["attr"][0] === "temperature") {
                temperature = response["queries"][0]["results"][i]["values"] || []
            }
            else if (response["queries"][0]["results"][i]["tags"]["attr"][0] === "humidity") {
                humidity = response["queries"][0]["results"][i]["values"] || []
            }
        }
    }
    return {
        dust: dust,
        temperature: temperature,
        humidity: humidity
    };
}


export { getStatus, getRecentFeed, getFeed }