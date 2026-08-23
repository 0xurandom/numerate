export interface CalcPage {
  id: string;
  name: string;
  input: string;
}

export function createCalcPage(name = "Untitled", input = ""): CalcPage {
  return {
    id: crypto.randomUUID(),
    name,
    input,
  };
}
