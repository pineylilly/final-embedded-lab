import { Menu } from '@headlessui/react'

function GraphTimeSwitch(props: {displayMode: string, handleDisplayMode: any}) {

    const setting = ["Realtime", "30 minutes", "1 hour", "2 hours", "3 hours", "6 hours", "12 hours", "1 day", "2 days", "5 days"]
        


    return (
        <div className="flex flex-row px-12 py-3 w-full items-center">
            <p className="font-bold pr-2">Display : </p>
            <Menu as="div" className="relative inline-block text-left">
            <Menu.Button className="normal-button">
                <span className="block">{props.displayMode}</span>
                
            </Menu.Button>
            <Menu.Items className="absolute mt-2 w-56 rounded-md bg-white dark:bg-gray-900 shadow-lg ring-1 ring-gray-100 ring-opacity-5 focus:outline-none z-10">
                {
                    setting.map((e) => {return (
                    <Menu.Item>
                        <a className="block px-4 py-2 text-sm text-gray-700 hover:bg-gray-100 dark:text-gray-400 dark:hover:bg-gray-600 dark:hover:text-white" role="menuitem" onClick={() => {props.handleDisplayMode(e)}}>
                            <div className="inline-flex items-center">
                                {e}
                            </div>
                        </a>
                    </Menu.Item>
                    )})
                }
                
                
            </Menu.Items>
            
        </Menu>
        </div>
    )
}

export default GraphTimeSwitch