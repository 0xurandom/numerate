import CodeMirror, { highlightActiveLine } from "@uiw/react-codemirror"
import { shadcnDark, shadcnLight } from "./editor-theme";
import { useTheme } from "@infogata/shadcn-vite-theme-provider";

interface InputTextViewProps {
  value: string;
  onChange: (value: string) => void;
  fontSize: number;
}

export function InputTextView({value, onChange, fontSize}: InputTextViewProps) {
  const { theme } = useTheme();
  const isDarkTheme = theme === "dark" || (theme == "system" && window.matchMedia("(prefers-color-scheme: dark)").matches);
  return (
    <CodeMirror
      theme={isDarkTheme ? shadcnDark : shadcnLight}
      value={ value }
      onChange={onChange}
      style={{
        fontFamily: "'Jetbrains Mono', monospace",
        fontSize: `${fontSize}px`
      }}
      height="100%"
      className="h-full"
      basicSetup={{
      lineNumbers: false,
      highlightActiveLine: false,
      foldGutter: false,
      closeBrackets: true,
      autocompletion: false,
    }}/>
  );
};

interface OutputTextViewProps {
  results: string[];
  fontSize: number;
}

export function OutputTextView({ results, fontSize }: OutputTextViewProps) {
  const { theme } = useTheme();
  const isDarkTheme = theme === "dark" || (theme == "system" && window.matchMedia("(prefers-color-scheme: dark)").matches);

  const outputString = results.join("\n");

  return (
    <CodeMirror
      value={ outputString }
      theme={isDarkTheme ? shadcnDark : shadcnLight}
      style={{
        fontFamily: "'Jetbrains Mono', monospace",
        fontSize: `${fontSize}px`
      }}
      height="100%"
      className="h-full"
      editable={false}
      basicSetup={{
      lineNumbers: false,
      highlightActiveLine: false,
      foldGutter: false,
      closeBrackets: true,
      autocompletion: false,
    }}/>
  );
}
