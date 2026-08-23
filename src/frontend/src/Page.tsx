export interface CalcPage {
  id: string;
  input: string;
}

export function createCalcPage(input = ""): CalcPage {
  return {
    id: crypto.randomUUID(),
    input,
  };
}
