import { IJSON } from "./IJSON";
import { IMode } from "./IMode";

export interface IHome extends IMode{
    data : Array<IJSON> | undefined
}