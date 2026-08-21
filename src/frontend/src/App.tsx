import { useState } from "react";
import reactLogo from "./assets/react.svg";
import { invoke } from "@tauri-apps/api/core";
import "./App.css";
import { NotebookView } from "./NotebookView";
import { Button } from "@/components/ui/button";
import MenuBar from "@/Menubar"
import { SplitView } from "./SplitView";


export default function App() {
  const [inputContent, setInputContent] = useState("");

  return (
    <main className="w-screen h-screen overflow-hidden">
      <SplitView value={inputContent} onChange={setInputContent} />
    </main>
  );
}
