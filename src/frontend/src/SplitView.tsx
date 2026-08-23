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

  const defaultFontSize: number = 14;
  const [fontSize, setFontSize] = useState(defaultFontSize);

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
    const handleKeyDown = (event: KeyboardEvent) => {
      if (!event.ctrlKey && !event.metaKey) return;

      if (event.key === "+" || event.key === "=") {
        event.preventDefault();
        setFontSize((size) => Math.min(size + 1, 32));
      }

      if (event.key === "-") {
        event.preventDefault();

        setFontSize((size) => Math.max(size - 1, 0));
      }

      if (event.key === "0") {
        event.preventDefault();
        setFontSize(14);
      }

    }
  })

  useEffect(() => {
    let isCurrent = true;

    const timer = setTimeout(() => {
      const evaluateText = async () => {
        const lines = value.split('\n');
        const newOutput: string[] = [];

        for (const line of lines) {
          if (!line.trim()) {
            newOutput.push("");
            continue;
          }

          try {
            const result: string = await invoke("evaluate", { input: line });
            newOutput.push(result);
          } catch {
            newOutput.push("Error");
          }
        }
        if (isCurrent) { setResults(newOutput); }
      };
      evaluateText();
    }, 200);


    return () => {
      isCurrent = false;
    };
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
          <InputTextView value={value} onChange={onChange} fontSize={fontSize} />
        </div>
        <div className="w-px h-full bg-gray-300 dark:bg-gray-700 shrink-0"/>
        <div
          ref={outputScrollRef}
          onScroll={handleOutputScroll}
          className="h-full overflow-auto transition-[width]  duration-250 ease-out"
           style={{width: WIDTH[page].output}}
        >

          <OutputTextView results={results} fontSize={fontSize} />
        </div>
      </div>
    </div>
  )
}
