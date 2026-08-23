import { useEffect, useRef, useState } from "react";
import "./App.css";
import { SplitView } from "./SplitView";
import { CalcPage, createCalcPage } from "./Page";
import { StatusBar } from "./StatusBar";


export default function App() {

  const [pages, setPages] = useState<CalcPage[]>(() => {
    const saved = localStorage.getItem("calc-pages");
    if (saved) {
      try {


        const parsed = JSON.parse(saved);
        if (Array.isArray(parsed) && parsed.length > 0) {
          return parsed;
        }
      } catch {
        console.error("Failed to load saved pages");
      }
    }

    return [createCalcPage()];
  });

  const swipeDelta = useRef(0);
  const swipeCooldown = useRef(false);

  useEffect(() => {
    const handleWheel = (event: WheelEvent) => {

      if (swipeCooldown.current) return;

      if (Math.abs(event.deltaX) <= Math.abs(event.deltaY)) {
        return;
      }

      event.preventDefault();

      swipeDelta.current += event.deltaX;

      const threshold = 80;
      if (swipeDelta.current > threshold) {
        swipeCooldown.current = true;

        setCurrentPage((current) => Math.max(current - 1, 0));
        swipeDelta.current = 0;

        setTimeout(() => {
          swipeCooldown.current = false;
        }, 300);
      } else if (swipeDelta.current < -threshold) {
        swipeCooldown.current = true;

        setCurrentPage((current) => Math.min(current + 1, pages.length - 1));
        swipeDelta.current = 0;

        setTimeout(() => {
          swipeCooldown.current = false;
        }, 300);
      }
    };

    window.addEventListener("wheel", handleWheel, {
      passive: false,
    });

    return () => {
      window.removeEventListener("wheel", handleWheel);
    };

  }, [pages.length]);

  const [currentPage, setCurrentPage] = useState(() => {
    const saved = localStorage.getItem("calc-current-page");

    const page = saved ? Number(saved) : 0;
    return Number.isInteger(page) && page >= 0 ? page : 0;
  });

  useEffect(() => {
    if (currentPage >= pages.length) {
      setCurrentPage(Math.max(0, pages.length - 1));
    }
  }, [currentPage, pages.length]);

  useEffect(() => {
    localStorage.setItem("calc-pages", JSON.stringify(pages));
  }, [pages]);

  useEffect(() => {
    localStorage.setItem("calc-current-page", currentPage.toString());
  }, [currentPage]);

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

        setPages((pages) => {
          const newPages = [...pages, createCalcPage()];
          setCurrentPage(newPages.length - 1);
          return newPages;
        });
      }

    };

    window.addEventListener("keydown", handleKeyDown);

    return () => {
      window.removeEventListener("keydown", handleKeyDown);
    };
  }, [pages.length]);


  const currentValue = pages[currentPage]?.input ?? "";
  const currentPageName = pages[currentPage]?.name ?? `Page ${currentPage + 1}`;


  const setCurrentValue = (input: string) => {
    setPages((pages) => pages.map((page, index) =>
      index === currentPage
        ? { ...page, input }
        : page));
  };

  const setCurrentPageName = (name: string) => {
    setPages((pages) =>
      pages.map((page, index) =>
        index == currentPage
          ? { ...page, name }
          : page
      )
    );
  }


  return (
    <main className="flex w-screen h-screen flex-col overflow-hidden">
      <div className="min-h-0 flex-1">
        <SplitView value={currentValue} onChange={setCurrentValue} />
      </div>
      <StatusBar
        pageName={currentPageName}
        currentPage={currentPage}
        pageCount={pages.length}
        onPageNameChange={setCurrentPageName}
      />
    </main>
  );
}
