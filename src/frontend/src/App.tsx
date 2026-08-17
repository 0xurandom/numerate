import { useState } from "react";
import reactLogo from "./assets/react.svg";
import { invoke } from "@tauri-apps/api/core";
import "./App.css";

import { Button } from "@/components/ui/button";
import {MenuBar} from "@/Menubar"
import { TextView } from "@/TextView";


export default function App() {
  const [greetMsg, setGreetMsg] = useState("");
  const [name, setName] = useState("");

  async function greet() {
    setGreetMsg(await invoke("greet", { name }));
  }

  async function hello(): Promise<String> {
    return await invoke("hello", { name });
  }

  return (
    <main className="container">
      <MenuBar/>
      <TextView/>
    </main>
  );
}
