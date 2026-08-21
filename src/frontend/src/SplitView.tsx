import { useEffect, useRef, useState } from "react";
import { InputTextView, OutputTextView } from "./TextView";
import { invoke } from "@tauri-apps/api/core";


interface SplitCalcViewProps {
  value: string;
  onChange: (value: string) => void;
}

export function SplitView({ value, onChange }: SplitCalcViewProps) {

  type Page = "input" | "split" | "output";
  const PAGES: Page[] = ["input", "split", "output"];

  const WIDTH: Record<Page, { input: string; output: string }> = {
    input: { input: "100%", output: "0%" },
    split: { input: "70%", output: "30%" },
    output: { input: "0%", output: "0%" },
  };

  const page = PAGES[1];

  const [results, setResults] = useState<string[]>([]);

  const containerRef = useRef<HTMLDivElement>(null);
  const inputScrollRef = useRef<HTMLDivElement>(null);
  const outputScrollRef = useRef<HTMLDivElement>(null);
  const syncScroll = useRef(false);

  const handleInputScroll = () => {
    if (syncScroll.current || !inputScrollRef.current || !outputScrollRef.current) return;
    syncScroll.current = true;
    outputScrollRef.current.scrollTop = inputScrollRef.current.scrollTop;
    syncScroll.current = false;
  };

  const handleOutputScroll = () => {
    if (syncScroll.current || !inputScrollRef.current || !outputScrollRef.current) return;
    syncScroll.current = true;
    inputScrollRef.current.scrollTop = outputScrollRef.current.scrollTop;
    syncScroll.current = false;
  };

  useEffect(() => {
    const lines = value.split("\n");
    let cancelled: boolean = false;
    (async () => {
      try {
        await invoke("reset_calc");
      } catch (err) {
        console.error("Failed to reset calculator state:", err);
      }

      const output: string[] = [];

      for (const line of lines) {
        if (line.trim() === "") { output.push(""); continue; }

        try {
          const result = await invoke<string>("evaluate", { input: line });
          output.push(result);
        } catch {
          output.push("Error");
        }
      }

      if (!cancelled) setResults(output);
    })();

    return () => { cancelled = true };
  }, [value]);

  return (
    <div ref={containerRef} className="w-full h-full overflow-hidden" >
      <div className="flex h-full">
        <div
          ref={inputScrollRef}
          onScroll={handleInputScroll}
          className="h-full overflow-auto transition-[width]  duration-250 ease-out"
          style={{ width: WIDTH[page].input }}
        >
          <InputTextView value={value} onChange={onChange} />
        </div>
        <div
          ref={outputScrollRef}
          onScroll={handleOutputScroll}
          className="h-full overflow-auto transition-[width]  duration-250 ease-out"
           style={{width: WIDTH[page].output}}
        >
           
          <OutputTextView />
        </div>
      </div>
    </div>
  )
}
