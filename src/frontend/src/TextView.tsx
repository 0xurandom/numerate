import CodeMirror, { highlightActiveLine } from "@uiw/react-codemirror"
import { shadcnDark, shadcnLight } from "./editor-theme";
import { useTheme } from "@infogata/shadcn-vite-theme-provider";

interface InputTextViewProps {
  value: string;
  onChange: (value: string) => void;
}

export function InputTextView({value, onChange}: InputTextViewProps) {
  const { theme } = useTheme();
  const isDarkTheme = theme === "dark" || (theme == "system" && window.matchMedia("(prefers-color-scheme: dark)").matches);
  return (
    <CodeMirror
      theme={isDarkTheme ? shadcnDark : shadcnLight}
      value={ value }
      onChange = { onChange }
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

interface OutputTextViewProps {
  results: string[];
}

export function OutputTextView({ results }: OutputTextViewProps) {
  const { theme } = useTheme();
  const isDarkTheme = theme === "dark" || (theme == "system" && window.matchMedia("(prefers-color-scheme: dark)").matches);

  const outputString = results.join("\n");

  return (
    <CodeMirror
      value={ outputString }
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
}
