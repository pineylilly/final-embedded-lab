import SVGlogo from "../../resources/logo/SVGlogo.svg"

function Header() {

    return (
        <nav className="bg-white px-2 sm:px-4 py-2 sticky w-full z-20 top-0 left-0 border-b border-gray-200">
            <div className="container flex flex-wrap items-center justify-between mx-auto">
            <a href="#" className="ml-2 md:ml-0 flex items-center">
                <img src={SVGlogo} className="h-6 mr-3 sm:h-9" alt="Logo" />
                <span className="self-center text-xl font-semibold whitespace-nowrap">FC Ajarn Yam</span>
            </a>
            <div className="items-center justify-between flex w-auto order-1" id="navbar-sticky">
              <ul className="flex flex-row p-4 rounded-lg bg-white space-x-4 md:space-x-8 mt-0 text-sm font-medium">
                <li>
                  <a href="#" className="block hover:text-sky-500 rounded">Dust</a>
                </li>
                <li>
                  <a href="#" className="block hover:text-sky-500 rounded">Temperature</a>
                </li>
                <li>
                  <a href="#" className="block hover:text-sky-500 rounded">Humidity</a>
                </li>
              </ul>
            </div>
            </div>
        </nav>
    )
}

export default Header