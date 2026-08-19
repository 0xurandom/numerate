import CodeMirror, { highlightActiveLine } from "@uiw/react-codemirror"
import { shadcnDark, shadcnLight } from "./editor-theme";
import { useTheme } from "@infogata/shadcn-vite-theme-provider";


export function TextView() {
  const { theme } = useTheme();
  const isDarkTheme = theme === "dark" || (theme == "system" && window.matchMedia("(prefers-color-scheme: dark)").matches);
  return (
    <CodeMirror
      theme={isDarkTheme ? shadcnDark : shadcnLight}
      height="100%"
      className="text-sm h-full"
      basicSetup={{
      lineNumbers: false,
      highlightActiveLine: false,
      foldGutter: false,
      closeBrackets: true,
      autocompletion: false,
    }}/>
  );
};
