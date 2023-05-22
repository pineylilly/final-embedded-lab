export interface IBook {
  queries: Query[];
}

interface Query {
  sample_size: number;
  results: Result[];
}

interface Result {
  name: string;
  group_by: Groupby[];
  tags: Tags;
  values: number[][];
}

interface Tags {
  attr: string[];
}

interface Groupby {
  name: string;
  tags?: string[];
  group?: Group;
  type?: string;
}

interface Group {
  attr: string;
}
