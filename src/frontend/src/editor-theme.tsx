import { createTheme } from '@uiw/codemirror-themes'

export const shadcnDark = createTheme(
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
})

export const shadcnLight = createTheme(
  {
    theme: 'light',
    settings: {
      background: 'hsl(var(--background))',
      foreground: 'hsl(var(--foreground))',
      selection: 'hsl(var(--muted))',
      gutterBackground: 'hsl(var(--background))',
      gutterForeground: 'hsl(var(--muted-foreground))',
      lineHighlight: 'transparent',
    },
    styles: []
})
