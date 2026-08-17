import { createTheme } from '@uiw/codemirror-themes'

export const shadcnTheme = createTheme(
  {
    theme: 'dark',
    settings: {
      background: 'hsl(var(--background))',
      foreground: 'hsl(var(--foreground))',
      selection: 'hsl(var(--muted))',
      gutterBackground: 'hsl(var(--background))',
      gutterForeground: 'hsl(var(--muted-foreground))',
      lineHighlight: 'transparent',
    },
    styles: []
}

)
