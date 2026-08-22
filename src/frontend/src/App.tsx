import { useState } from "react";
import "./App.css";
import { SplitView } from "./SplitView";


export default function App() {
  const [inputContent, setInputContent] = useState("");

  return (
    <main className="w-screen h-screen overflow-hidden">
      <SplitView value={inputContent} onChange={setInputContent} />
    </main>
  );
}
