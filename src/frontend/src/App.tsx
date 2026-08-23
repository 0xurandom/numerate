import { useEffect, useState } from "react";
import "./App.css";
import { SplitView } from "./SplitView";
import { CalcPage, createCalcPage } from "./Page";


export default function App() {

  const [pages, setPages] = useState<CalcPage[]>();


  const [currentPage, setCurrentPage] = useState(0);

  useEffect(() => {
    const handleKeyDown = (event: KeyboardEvent) => {
      if (!event.ctrlKey && !event.metaKey) return;

      if (event.key === "ArrowRight") {
        event.preventDefault();

        setCurrentPage((current) => Math.min(current + 1, pages.length - 1));
      }

      if (event.key == "ArrowLeft") {
        event.preventDefault();

        setCurrentPage((current) => Math.max(current - 1, 0));
      }

      if (event.key.toLowerCase() === "n") {
        event.preventDefault();

        const newPage = createCalcPage();


        setPages((pages) => [...pages, newPage]);
        setCurrentPage(pages.length);
      }

    };

    window.addEventListener("keydown", handleKeyDown);

    return () => {
      window.removeEventListener("keydown", handleKeyDown);
    };
  }, [pages.length]);

  const [inputContent, setInputContent] = useState("");

  return (
    <main className="w-screen h-screen overflow-hidden">
      <SplitView value={inputContent} onChange={setInputContent} />
    </main>
  );
}
